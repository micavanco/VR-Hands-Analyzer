#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_chartLeft = nullptr;
    m_chartRight = nullptr;

    connect(&m_cameraThread, SIGNAL(CameraStatus(QString)), ui->cameraStatusLabel, SLOT(setText(QString)));
    connect(&m_cameraThread, SIGNAL(CoordinatesRight(QString)), ui->coordinatesRightLabel, SLOT(setText(QString)));
    connect(&m_cameraThread, SIGNAL(CoordinatesLeft(QString)), ui->coordinatesLeftLabel, SLOT(setText(QString)));
}

MainWindow::~MainWindow()
{
    if(m_chartLeft != nullptr)
        delete m_chartLeft;
    if(m_chartRight != nullptr)
        delete m_chartRight;
    m_cameraThread.requestInterruption();
    m_cameraThread.wait();
    delete ui;
}


void MainWindow::on_turnOnButton_pressed()
{
    m_cameraThread.start();
    ui->recordButton->setEnabled(true);
    ui->turnOnButton->setEnabled(false);
    ui->turnOffButton->setEnabled(true);
}

void MainWindow::on_turnOffButton_pressed()
{
    m_cameraThread.requestInterruption();
    m_cameraThread.wait();
    ui->turnOnButton->setEnabled(true);
    ui->turnOffButton->setEnabled(false);
    ui->recordButton->setEnabled(false);
}

void MainWindow::on_recordButton_pressed()
{
    // pobierz aktualny czas oraz utwórz nowy plik tekstowy z datą w nazwie
    fileTitle = QTime::currentTime().toString("hh_mm_ss")+QString("[%1][").arg(ui->attemptSpinBox->value())+ui->patientLineEdit->text()+"]";
    file = new QFile("output/"+fileTitle+".csv");
    file->open(QIODevice::ReadWrite | QIODevice::Text);
    // utwórz obiekt do transmisji danych do pliku oraz prześlij dwa wiersze
    streamOut = new QTextStream(file);
    *streamOut << QTime::currentTime().toString("hh:mm:ss")+"\n";
    *streamOut << QString("%1").arg((int)ui->timeSpinBox->value()*10)+"\n";
    *streamOut << "Time;;Left Hand;;;;;Right Hand\n";
    *streamOut << "[s];[x];[y];[z];["+QString(176)+"];;[x];[y];[z];["+QString(176)+"]\n";
    // utwórz nowy obiekt zliczający czas oraz połącz go z metodą wywoływaną po każdym upływie 100 ms
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    // połączenie sygnału osiągnięcia maksymalnego czasu przez licznik z metodą
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    time = ui->timeSpinBox->value();
    totalTime = time;
    ui->timeToEndLabel->setText(QString("%1").arg(time));
    // zacznij zliczać czas
    timer->start(100);
    ui->timeSpinBox->setEnabled(false);
    ui->turnOffButton->setEnabled(false);
    ui->recordButton->setEnabled(false);
    timeCount = time*10+1;
}

void MainWindow::onTimerTimeout()
{
    // zapisz do pliku aktualne położenie obiektów
    *streamOut << QString("%1").arg(totalTime-time)+";"+ui->coordinatesLeftLabel->text().split(" ")[1]+";"+
                ui->coordinatesLeftLabel->text().split(" ")[3]+";"+ui->coordinatesLeftLabel->text().split(" ")[5]
                +";"+ui->coordinatesLeftLabel->text().split(" ")[7]+";"+
                +";"+ui->coordinatesRightLabel->text().split(" ")[1]+";"+
            ui->coordinatesRightLabel->text().split(" ")[3]+";"+ui->coordinatesRightLabel->text().split(" ")[5]
            +";"+ui->coordinatesRightLabel->text().split(" ")[7]+";"+
            +"\n";
    time -= 0.1;
    timeCount--;
    ui->timeToEndLabel->setText(QString("%1").arg(time));
    if(timeCount == 0)// jeżeli zliczany czas wynosi zero to zatrzymaj licznik i transmisję danych do pliku oraz usuń związane z tym obiekty
    {
        timer->stop();
        file->flush();
        file->close();
        delete file;
        delete streamOut;
        ui->timeSpinBox->setEnabled(true);
        ui->turnOffButton->setEnabled(true);
        ui->recordButton->setEnabled(true);
        ui->timeToEndLabel->setText("Zapisano do "+fileTitle+".csv");
    }
}


void MainWindow::on_loadPatientFileButton_pressed()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", "Text File (*.csv)");

    if(filename.isEmpty())return;

    file = new QFile(filename);
        file->open(QIODevice::ReadOnly | QIODevice::Text);

    QStringList filenames = filename.split("/");
    filename = filenames[filenames.length()-1];

    if(m_filePatientStorageLeftX.contains(filename))
    {
        ui->filePatientLabelInfo->setText("This file already exists!");
        return;
    }

    streamOut = new QTextStream(file);
    streamOut->readLine();

    int fileLength = streamOut->readLine().toInt();

    if(fileLength == 0)// jeżeli pierwszy wiersz nie zawiera liczby, bądź tą liczbą jest zero to zakończ wczytywanie pliku
    {
        ui->filePatientLabelInfo->setText("Invalid file.\nTry again.");
        return;
    }

    ui->filePatientLabelInfo->setText("File loaded successfully!");

    m_filePatientStorageLength.insert(filename, fileLength);

    streamOut->readLine();
    streamOut->readLine();
    QString line;

    QVector<double> arrayLeftX;
    QVector<double> arrayLeftY;
    QVector<double> arrayLeftZ;
    QVector<double> arrayLeftDeg;
    QVector<double> arrayRightX;
    QVector<double> arrayRightY;
    QVector<double> arrayRightZ;
    QVector<double> arrayRightDeg;
    QVector<double> arrayTime;
    for(int i=0; i < fileLength+1; i++)
    {
        // wczytanie całej linii wiersza oraz odseparowania danych od siebie i zapis do tablicy
        line = streamOut->readLine();
        QStringList list = line.split(";");
        arrayTime.append(list[0].toDouble());
        arrayLeftX.append(list[1].toDouble());
        arrayLeftY.append(list[2].toDouble());
        arrayLeftZ.append(list[3].toDouble());
        arrayLeftDeg.append(list[4].toDouble());
        arrayRightX.append(list[6].toDouble());
        arrayRightY.append(list[7].toDouble());
        arrayRightZ.append(list[8].toDouble());
        arrayRightDeg.append(list[9].toDouble());
    }

    m_filePatientStorageLeftX.insert(filename, arrayLeftX);
    m_filePatientStorageLeftY.insert(filename, arrayLeftY);
    m_filePatientStorageLeftZ.insert(filename, arrayLeftZ);
    m_filePatientStorageLeftDeg.insert(filename, arrayLeftDeg);

    m_filePatientStorageRightX.insert(filename, arrayRightX);
    m_filePatientStorageRightY.insert(filename, arrayRightY);
    m_filePatientStorageRightZ.insert(filename, arrayRightZ);
    m_filePatientStorageRightDeg.insert(filename, arrayRightDeg);

    m_filePatientStorageTime.insert(filename, arrayTime);

    ui->fileComboBox->addItem(filename);


    file->close();
    delete streamOut;
    delete file;

    ui->chartButton->setEnabled(true);
    ui->analysisButton->setEnabled(true);
}

void MainWindow::on_deletePatientFileButton_pressed()
{
    QString filename = ui->fileComboBox->currentText();

    m_filePatientStorageLeftX.find(filename).value().clear();
    m_filePatientStorageLeftY.find(filename).value().clear();
    m_filePatientStorageLeftZ.find(filename).value().clear();
    m_filePatientStorageLeftDeg.find(filename).value().clear();

    m_filePatientStorageRightX.find(filename).value().clear();
    m_filePatientStorageRightY.find(filename).value().clear();
    m_filePatientStorageRightZ.find(filename).value().clear();
    m_filePatientStorageRightDeg.find(filename).value().clear();

    m_filePatientStorageTime.find(filename).value().clear();

    //------------------------------------------------------------
    m_filePatientStorageLeftX.remove(filename);
    m_filePatientStorageLeftY.remove(filename);
    m_filePatientStorageLeftZ.remove(filename);
    m_filePatientStorageLeftDeg.remove(filename);

    m_filePatientStorageRightX.remove(filename);
    m_filePatientStorageRightY.remove(filename);
    m_filePatientStorageRightZ.remove(filename);
    m_filePatientStorageRightDeg.remove(filename);

    m_filePatientStorageTime.remove(filename);

    m_filePatientStorageLength.remove(filename);

    ui->fileComboBox->removeItem(ui->fileComboBox->currentIndex());

    m_filePatientStorageLength.remove(filename);

    ui->filePatientLabelInfo->setText("File deleted successfully!");

    if(ui->fileComboBox->currentText() == "")
    {
        ui->chartButton->setEnabled(false);
        ui->analysisButton->setEnabled(false);
    }

}

void MainWindow::on_restoreButton_pressed()
{
    ui->chartViewLeft->setVisible(true);
    ui->chartViewLeft->setFloating(false);
    ui->chartViewRight->setVisible(true);
    ui->chartViewRight->setFloating(false);
}

void MainWindow::on_fileComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "")
        ui->chartButton->setEnabled(false);
    else
        ui->chartButton->setEnabled(true);
}

void MainWindow::on_chartButton_pressed()
{
    if(m_seriesLeft.keys().length() != 0)
    {
        for(auto e: m_seriesLeft.keys())
        {
            delete m_seriesLeft.value(e);
            m_seriesLeft.remove(e);
        }
        for(auto e: m_seriesRight.keys())
        {
            delete m_seriesRight.value(e);
            m_seriesRight.remove(e);
        }
    }

    if(m_chartLeft != nullptr)
        delete m_chartLeft;

    if(m_chartRight != nullptr)
        delete m_chartRight;

    if(ui->radioButtonX->isChecked())
    {
        int max = 0 ;
        double leftMax = 0.0;
        double rightMax = 0.0;
        for(auto e: m_filePatientStorageTime.keys())
        {
            QLineSeries *seriesLeft = new QLineSeries();
            QLineSeries *seriesRight = new QLineSeries();
            int temp = 0;
            for(auto i: m_filePatientStorageTime.value(e))
            {
                seriesLeft->append(i, m_filePatientStorageLeftX.value(e).at(i*10));
                if(leftMax < qAbs(m_filePatientStorageLeftX.value(e).at(i*10)) )
                    leftMax = qAbs(m_filePatientStorageLeftX.value(e).at(i*10));
            }


            for(auto i: m_filePatientStorageTime.value(e))
            {
                seriesRight->append(i, m_filePatientStorageRightX.value(e).at(i*10));
                if(rightMax < qAbs(m_filePatientStorageRightX.value(e).at(i*10)))
                    rightMax = qAbs(m_filePatientStorageRightX.value(e).at(i*10));
            }


            m_seriesLeft.insert(e, seriesLeft);
            m_seriesRight.insert(e, seriesRight);
            if(max < m_filePatientStorageTime.value(e).length())
                max = m_filePatientStorageTime.value(e).length()/10;
        }

        m_chartLeft = new Chart(m_seriesLeft, "Left hand", "[s]", "axis X", max, leftMax, true);
        m_chartRight = new Chart(m_seriesRight, "Right hand", "[s]", "axis X", max, rightMax, true);

        ui->chartViewLeft->setWidget(m_chartLeft);
        ui->chartViewRight->setWidget(m_chartRight);
    }else if(ui->radioButtonY->isChecked())
    {
        int max = 0 ;
        double leftMax = 0.0;
        double rightMax = 0.0;
        for(auto e: m_filePatientStorageTime.keys())
        {
            QLineSeries *seriesLeft = new QLineSeries();
            QLineSeries *seriesRight = new QLineSeries();
            int temp = 0;
            for(auto i: m_filePatientStorageTime.value(e))
            {
                seriesLeft->append(i, m_filePatientStorageLeftY.value(e).at(i*10));
                if(leftMax < qAbs(m_filePatientStorageLeftY.value(e).at(i*10)) )
                    leftMax = qAbs(m_filePatientStorageLeftY.value(e).at(i*10));
            }


            for(auto i: m_filePatientStorageTime.value(e))
            {
                seriesRight->append(i, m_filePatientStorageRightY.value(e).at(i*10));
                if(rightMax < qAbs(m_filePatientStorageRightY.value(e).at(i*10)))
                    rightMax = qAbs(m_filePatientStorageRightY.value(e).at(i*10));
            }


            m_seriesLeft.insert(e, seriesLeft);
            m_seriesRight.insert(e, seriesRight);
            if(max < m_filePatientStorageTime.value(e).length())
                max = m_filePatientStorageTime.value(e).length()/10;
        }

        m_chartLeft = new Chart(m_seriesLeft, "Left hand", "[s]", "axis Y", max, leftMax, false);
        m_chartRight = new Chart(m_seriesRight, "Right hand", "[s]", "axis Y", max, rightMax, false);

        ui->chartViewLeft->setWidget(m_chartLeft);
        ui->chartViewRight->setWidget(m_chartRight);
    }else if(ui->radioButtonZ->isChecked())
    {
        int max = 0 ;
        double leftMax = 0.0;
        double rightMax = 0.0;
        for(auto e: m_filePatientStorageTime.keys())
        {
            QLineSeries *seriesLeft = new QLineSeries();
            QLineSeries *seriesRight = new QLineSeries();
            int temp = 0;
            for(auto i: m_filePatientStorageTime.value(e))
            {
                seriesLeft->append(i, m_filePatientStorageLeftZ.value(e).at(i*10));
                if(leftMax < qAbs(m_filePatientStorageLeftZ.value(e).at(i*10)) )
                    leftMax = qAbs(m_filePatientStorageLeftZ.value(e).at(i*10));
            }


            for(auto i: m_filePatientStorageTime.value(e))
            {
                seriesRight->append(i, m_filePatientStorageRightZ.value(e).at(i*10));
                if(rightMax < qAbs(m_filePatientStorageRightZ.value(e).at(i*10)))
                    rightMax = qAbs(m_filePatientStorageRightZ.value(e).at(i*10));
            }


            m_seriesLeft.insert(e, seriesLeft);
            m_seriesRight.insert(e, seriesRight);
            if(max < m_filePatientStorageTime.value(e).length())
                max = m_filePatientStorageTime.value(e).length()/10;
        }

        m_chartLeft = new Chart(m_seriesLeft, "Left hand", "[s]", "axis Z", max, leftMax, true);
        m_chartRight = new Chart(m_seriesRight, "Right hand", "[s]", "axis Z", max, rightMax, true);

        ui->chartViewLeft->setWidget(m_chartLeft);
        ui->chartViewRight->setWidget(m_chartRight);
    }else if(ui->radioButtonDeg->isChecked())
    {
        int max = 0 ;
        double leftMax = 0.0;
        double rightMax = 0.0;
        for(auto e: m_filePatientStorageTime.keys())
        {
            QLineSeries *seriesLeft = new QLineSeries();
            QLineSeries *seriesRight = new QLineSeries();
            int temp = 0;
            for(auto i: m_filePatientStorageTime.value(e))
            {
                seriesLeft->append(i, m_filePatientStorageLeftDeg.value(e).at(i*10));
                if(leftMax < qAbs(m_filePatientStorageLeftDeg.value(e).at(i*10)) )
                    leftMax = qAbs(m_filePatientStorageLeftDeg.value(e).at(i*10));
            }


            for(auto i: m_filePatientStorageTime.value(e))
            {
                seriesRight->append(i, m_filePatientStorageRightDeg.value(e).at(i*10));
                if(rightMax < qAbs(m_filePatientStorageRightDeg.value(e).at(i*10)))
                    rightMax = qAbs(m_filePatientStorageRightDeg.value(e).at(i*10));
            }


            m_seriesLeft.insert(e, seriesLeft);
            m_seriesRight.insert(e, seriesRight);
            if(max < m_filePatientStorageTime.value(e).length())
                max = m_filePatientStorageTime.value(e).length()/10;
        }

        m_chartLeft = new Chart(m_seriesLeft, "Left hand", "[s]", "axis Degree", max, leftMax, false);
        m_chartRight = new Chart(m_seriesRight, "Right hand", "[s]", "axis Degree", max, rightMax, false);

        ui->chartViewLeft->setWidget(m_chartLeft);
        ui->chartViewRight->setWidget(m_chartRight);
    }
}

void MainWindow::on_chartViewLeft_visibilityChanged(bool visible)
{
    if(ui->chartViewLeft->isFloating() || ui->chartViewLeft->isHidden() ||
            ui->chartViewRight->isFloating() || ui->chartViewRight->isHidden())
        ui->restoreButton->setEnabled(true);
    else
        ui->restoreButton->setEnabled(false);
}

void MainWindow::on_chartViewRight_visibilityChanged(bool visible)
{
    if(ui->chartViewLeft->isFloating() || ui->chartViewLeft->isHidden() ||
            ui->chartViewRight->isFloating() || ui->chartViewRight->isHidden())
        ui->restoreButton->setEnabled(true);
    else
        ui->restoreButton->setEnabled(false);
}

void MainWindow::on_analysisButton_pressed()
{
    qreal sumXleft;
    qreal sumYleft;
    qreal sumZleft;
    qreal sumDegleft;

    qreal sumXright;
    qreal sumYright;
    qreal sumZright;
    qreal sumDegright;

    double dx;

    // pobierz aktualny czas oraz utwórz nowy plik tekstowy z datą w nazwie
    fileTitle = QTime::currentTime().toString("hh_mm_ss_analysis")+QString("[%1][").arg(ui->attemptSpinBox->value())+ui->patientLineEdit->text()+"]";
    file = new QFile("analysis/"+fileTitle+".csv");
    file->open(QIODevice::ReadWrite | QIODevice::Text);
    // utwórz obiekt do transmisji danych do pliku oraz prześlij dwa wiersze
    streamOut = new QTextStream(file);
    *streamOut << QTime::currentTime().toString("hh:mm:ss")+"\n";
    *streamOut << "Left Hand;;;;;Right Hand\n";
    *streamOut << "[x];[y];[z];["+QString(176)+"];;[x];[y];[z];["+QString(176)+"]\n";

    for(auto e: m_filePatientStorageTime.keys())
    {
        for(int i = 0; i < m_filePatientStorageTime.value(e).length()-1; i++)
        {
            dx = qAbs(m_filePatientStorageLeftX.value(e).at(i))-qAbs(m_filePatientStorageLeftX.value(e).at(i+1));
            dx = qSqrt(dx*dx+1);
            if(qIsNaN(dx))
                sumXleft += dx;
            dx = qAbs(m_filePatientStorageLeftY.value(e).at(i))-qAbs(m_filePatientStorageLeftY.value(e).at(i+1));
            dx = qSqrt(dx*dx+1);
            if(qIsNaN(dx))
                sumYleft += dx;
            dx = qAbs(m_filePatientStorageLeftZ.value(e).at(i))-qAbs(m_filePatientStorageLeftZ.value(e).at(i+1));
            dx = qSqrt(dx*dx+1);
            if(qIsNaN(dx))
                sumZleft += dx;
            dx = qAbs(m_filePatientStorageLeftDeg.value(e).at(i))-qAbs(m_filePatientStorageLeftDeg.value(e).at(i+1));
            dx = qSqrt(dx*dx+1);
            if(qIsNaN(dx))
                sumDegleft += dx;

            dx = qAbs(m_filePatientStorageRightX.value(e).at(i))-qAbs(m_filePatientStorageRightX.value(e).at(i+1));
            dx = qSqrt(dx*dx+1);
            if(qIsNaN(dx))
                sumXright += dx;
            dx = qAbs(m_filePatientStorageRightY.value(e).at(i))-qAbs(m_filePatientStorageRightY.value(e).at(i+1));
            dx = qSqrt(dx*dx+1);
            if(qIsNaN(dx))
                sumYright += dx;
            dx = qAbs(m_filePatientStorageRightZ.value(e).at(i))-qAbs(m_filePatientStorageRightZ.value(e).at(i+1));
            dx = qSqrt(dx*dx+1);
            if(qIsNaN(dx))
                sumZright += dx;
            dx = qAbs(m_filePatientStorageRightDeg.value(e).at(i))-qAbs(m_filePatientStorageRightDeg.value(e).at(i+1));
            dx = qSqrt(dx*dx+1);
            if(qIsNaN(dx))
                sumDegright += dx;
        }
        *streamOut << QString("%1;%2;%3;%4;;%5;%6;%7;%8").arg(sumXleft)
                      .arg(sumYleft).arg(sumZleft).arg(sumDegleft)
                      .arg(sumXright)
                      .arg(sumYright).arg(sumZright).arg(sumDegright)+"\n";
    }
    ui->filePatientLabelInfo->setText("Analysis saved to file successfully!");
    file->flush();
    file->close();
    delete file;
    delete streamOut;
}
