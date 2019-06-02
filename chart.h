#ifndef CHART_H
#define CHART_H

#include <QGraphicsView>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

class Chart : public QChartView
{
    Q_OBJECT
public:
    explicit Chart(QMap<QString, QLineSeries*>series, QString title, QString axisXLabel, QString axisYLabel, int rangeX, int rangeY,
                   bool hasMinus = false, QWidget *parent = nullptr);
    ~Chart();

    double getSurfaceInfo1(){return m_surface1;}
    double getSurfaceInfo2(){return m_surface2;}
signals:

public slots:
    void showWindowCoord(QPointF point, bool state); // metoda odbierająca dane z sygnału najechania kursorem na wykres w postaci parametrów funkcji

private:
    int     m_countRotation;            // zmienna ograniczająca oddalanie widoku wykresu
    bool    m_isPressed;                // sprawdzenie czy jest wciśnięty klawisz
    QPoint  m_pos;                      // pozycja kursora po wciśnięciu lewego przycisku myszy
    int     m_rangeX;                   // maksymalny zakres osi x
    int     m_rangeY;                   // maksymalny zakres osi y
    QGraphicsTextItem *m_coord;         // okno wyświetlające położenie x i y nad którym znajduje się kursor
    QVector<QPen>   m_pen;
    QMap<QString, QLineSeries*> m_series;
    QChart* m_chart;
    int     m_i;
    double m_surface1;                  // pole powierzchni pierwszego przebiegu
    double m_surface2;                  // pole powierzchni drugiego przebiegu
    bool   m_hasMinus;


    void calculateIntegral();             // obliczenie pola powierzchni za pomocą metody całkowej
    void wheelEvent(QWheelEvent *event);        // metoda obsługi zdarzeń pokrętła myszki
    void mousePressEvent(QMouseEvent *event);   // metoda obsługi zdarzeń naciśnięcia klawisza myszki
    void mouseReleaseEvent(QMouseEvent *event); // metoda obsługi zdarzeń puszczenia klawisza myszki
    void mouseMoveEvent(QMouseEvent *event);    // metoda obsługi zdarzeń przesuwania myszki
};
#endif // CHART_H
