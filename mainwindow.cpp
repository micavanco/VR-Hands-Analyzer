#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_cameraThread, SIGNAL(CameraStatus(QString)), ui->cameraStatusLabel, SLOT(setText(QString)));
    connect(&m_cameraThread, SIGNAL(CoordinatesRight(QString)), ui->coordinatesRightLabel, SLOT(setText(QString)));
    connect(&m_cameraThread, SIGNAL(CoordinatesLeft(QString)), ui->coordinatesLeftLabel, SLOT(setText(QString)));
}

MainWindow::~MainWindow()
{
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
    fileTitle = QTime::currentTime().toString("hh_mm_ss")+"-"+ui->patientLineEdit->text()+"-attempt-"+QString("%1").arg(ui->attemptSpinBox->value());
    file = new QFile("output/"+fileTitle+".csv");
    file->open(QIODevice::ReadWrite | QIODevice::Text);
    // utwórz obiekt do transmisji danych do pliku oraz prześlij dwa wiersze
    streamOut = new QTextStream(file);
    *streamOut << QTime::currentTime().toString("hh:mm:ss")+"\n";
    *streamOut << QString("%1").arg((int)ui->timeSpinBox->value()*10)+"\n";
    *streamOut << "Time,,Left Hand,,,,,Right Hand\n";
    *streamOut << "[s],[x],[y],[z],["+QString(176)+"],,[x],[y],[z],["+QString(176)+"]\n";
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
    *streamOut << QString("%1").arg(totalTime-time)+","+ui->coordinatesLeftLabel->text().split(" ")[1]+","+
                ui->coordinatesLeftLabel->text().split(" ")[3]+","+ui->coordinatesLeftLabel->text().split(" ")[5]
                +","+ui->coordinatesLeftLabel->text().split(" ")[7]+","+
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

