#include "chart.h"

Chart::Chart(QMap<QString, QLineSeries*> series, QString title, QString axisXLabel, QString axisYLabel, int rangeX, int rangeY,
             bool hasMinus,QWidget *parent)
    : QChartView(parent), m_countRotation(0), m_isPressed(false),
      m_rangeX(rangeX), m_rangeY(rangeY), m_series(series), m_hasMinus(hasMinus)
{
        // przypisanie nazwy serii danych oraz dodanie do wykresu
    m_i = 0;
    m_chart = new QChart();
    for(auto e: series.keys())
    {
        QLineSeries* s = series.value(e);
        s->setName(e);
        QPen   pen = QPen(QColor(100+m_i, 50+m_i, m_i, 250-m_i), 2);
        m_pen.append(pen);
        s->setPen(pen);
        m_chart->addSeries(s);
        m_i+=60;
        connect(s, &QLineSeries::hovered, this, &Chart::showWindowCoord);
        m_series.insert(e, s);
    }

    // utworznie obiektu modyfikującego czcionkę
    QFont font;
    font.setPixelSize(18);
    m_chart->setTitleFont(font);    // przypisanie obiektu czcionki do tytułu wykresu
    m_chart->setTitleBrush(QBrush(Qt::gray)); // ustawienie kolory tytułu wykresu

    m_coord = new QGraphicsTextItem(m_chart); // utworzenie obiektu wyświetlającego koordynacje kursora na wykresie i przypisanie do wykresu
    m_coord->setZValue(11); // przeniesienie okna w górę hierarchii wyświetlanych elementów aby wykres nie przesłaniał

    m_chart->setTitle(title); // przypisanie łańcucha znaków do tytułu wykresu
    m_chart->createDefaultAxes(); // utworzenie domyślnych osi
    m_chart->setAnimationOptions(QChart::AllAnimations); // aktywacja animacji wszystkich elementów


    m_chart->axisX()->setRange(0,rangeX);   // ustawienie zakresu osi x
    m_chart->axisX()->setTitleText(axisXLabel); // ustawienie tytułu osi x

    if(hasMinus)
        m_chart->axisY()->setRange(-rangeY,rangeY);   // ustawienie zakresu osi y
    else
        m_chart->axisY()->setRange(0,rangeY);   // ustawienie zakresu osi y

        m_chart->axisY()->setTitleText(axisYLabel); // ustawienie tytułu osi y


    this->setChart(m_chart);    // przypisanie obiektu wykresu do sceny, czyli tej klasy
    this->setCursor(Qt::OpenHandCursor); // ustawienie kursora otwartej dłoni
}

Chart::~Chart()
{
    delete m_coord; // usuwanie obiektu wyświetlającego koordynacje kursora
    delete m_chart; // usuwanie obiektu wykresu
}
// metoda odbierająca dane z sygnału najechania kursorem na wykres w postaci parametrów funkcji
void Chart::showWindowCoord(QPointF point, bool state)
{
    if(state) // sprawdzenie stanu położenia kursora, czy znajduje się nad wykresem
    {
        if(point.x() > m_rangeX*0.75) // sprawdzenie czy kursor znajduje się w 75% długości osi x jeżeli tak to...
            m_coord->setPos(m_chart->mapToPosition(point).x()-150, m_chart->mapToPosition(point).y()-40);// zmieniamy położenie okna z koordynacjami na lewą stronę
        else    //                                                                                          położenia kursora
            m_coord->setPos(m_chart->mapToPosition(point).x()+15, m_chart->mapToPosition(point).y()-40);// jeżeli nie to na prawą stronę
        m_coord->show(); // wyświetlenie okna
        this->setCursor(Qt::CrossCursor); // ustawienie kursora krzyża
    }else // jeżeli kursor nie znajduje się nad wykresem to ...
    {
        this->setCursor(Qt::OpenHandCursor); // ustawienie kursora otwartej dłoni
        m_coord->hide(); // ukrycie okna
    }
}
// obliczanie pola powierzchni za pomocą metody całkowej - metoda trapezów
void Chart::calculateIntegral()
{
    /*double surface1 = 0, surface2 = 0;
    // dy - różnica pomiędzy y1, a y2
    double y1, y2, dy;
    for(int i = 0; i < m_rangeX; i++)
    {
        // obliczanie powierzchni dla serii 1
        y1 = m_series1->at(i).y();
        y2 = m_series1->at(i+1).y();
        dy = (y2 - y1)/2;
        surface1 += (y1 + dy);
        // obliczanie powierzchni dla serii 2
        y1 = m_series2->at(i).y();
        y2 = m_series2->at(i+1).y();
        dy = (y2 - y1)/2;
        surface2 += (y1 + dy);
    }

    m_surface1 = surface1;
    m_surface2 = surface2;*/
}
//  metoda obsługi zdarzeń pokrętła myszki
void Chart::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y() > 0)// jeżeli pokrętło jest kręcona od uzytkownika, to przyjmuje wartości dodatnie
    {
        chart()->zoomIn(); // przybliżenie wykresu
        m_countRotation++; // inkrementacja zmiennej przechowującej ilość wykonanych obrotów przybliżających
        for(auto e:m_pen)
            e.setWidth(m_countRotation+2);

        int i = 0;
        for(auto e:m_series.keys())
            m_series.value(e)->setPen(m_pen.at(i++));
    }
    else if(m_countRotation) // jeżeli zmienna przechowuje liczby dodatnie to...
    {
        chart()->zoomOut(); // oddal wykres
        m_countRotation--;  // dekrementacja liczb przybliżeń
        for(auto e:m_pen)
            e.setWidth(m_countRotation+2);

        int i = 0;
        for(auto e:m_series.keys())
            m_series.value(e)->setPen(m_pen.at(i++));

    }else // jeżeli warość przybliżeń wynosi zero to...
    {
        chart()->axisX()->setRange(0, m_rangeX); // ustaw zakres osi x na domyślny
        if(m_hasMinus)
            chart()->axisY()->setRange(-m_rangeY, m_rangeY); // ustaw zakres osi y na domyślny
        else
            chart()->axisY()->setRange(0, m_rangeY); // ustaw zakres osi y na domyślny
    }
    return QChartView::wheelEvent(event);
}
// metoda obsługi zdarzeń naciśnięcia klawisza myszki
void Chart::mousePressEvent(QMouseEvent *event)
{
    if(!m_isPressed) // jeżeli klawisz nie był wcześniej wciśnięty to...
    {
        m_isPressed = true;     // ustaw wartość informującą o wciśnięciu klawisza
        m_pos = event->pos();   // pobierz pozycję kursora
        this->setCursor(Qt::ClosedHandCursor); // ustawienie kursora zamkniętej dłoni
    }
    QChartView::mousePressEvent(event);
}
// metoda obsługi zdarzeń puszczenia klawisza myszki
void Chart::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isPressed) // jeżeli klawisz myszki był wciśnięty to...
    {
        chart()->setAnimationOptions(QChart::SeriesAnimations); // ustaw animację serii wykresu
        m_isPressed = false; // ustaw wartość informującą o puszczeniu klawisza
        this->setCursor(Qt::OpenHandCursor); // ustawienie kursora otwartej dłoni
    }
    QChartView::mouseReleaseEvent(event);
}
// metoda obsługi zdarzeń przesuwania myszki
void Chart::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed) // jeżeli klawisz myszki był wciśnięty to...
    {
        QPoint temp = event->pos();     // przechowaj aktualną pozycję kursora w zmiennej tymczasowej
        int deltaX = m_pos.x()-temp.x();        // obliczenie o ile przesunął się kursor od momentu przyciśnięcia klawisza myszki
        int deltaY = (m_pos.y()-temp.y())/-1;   // to samo dla osi y
        chart()->scroll(deltaX, deltaY);        // przesunięcie zakresu osi o wartości tego przesunięcia
        m_pos = temp;           // przypisanie aktualnej pozycji do zmiennej klasy
    }
    m_coord->setHtml("<div style='background-color: #ffffff; font-size: 15px;'>"+QString("x: %1\ny: %2")
                     .arg(m_chart->mapToValue(event->pos()).x())
                     .arg(m_chart->mapToValue(event->pos()).y())+"</div>");     // przypisanie koordynacji kursora do okna z informacją o położeniu
    QChartView::mouseMoveEvent(event);                                          // w postaci kodu html w celu przypisania koloru tła
}