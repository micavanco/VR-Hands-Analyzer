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

    m_filePatientStorageRightX.insert(filename, arrayLeftX);
    m_filePatientStorageRightY.insert(filename, arrayLeftY);
    m_filePatientStorageRightZ.insert(filename, arrayLeftZ);
    m_filePatientStorageRightDeg.insert(filename, arrayLeftDeg);

    m_filePatientStorageTime.insert(filename, arrayTime);

    ui->fileComboBox->addItem(filename);


    file->close();
    delete streamOut;
    delete file;
}

void MainWindow::on_deletePatientFileButton_pressed()
{
    QString filename = ui->fileComboBox->currentText();

    for(int i = 0; i < m_filePatientStorageLength.find(filename).value()+1; i++)
       qDebug() << m_filePatientStorageLeftX.find(filename).value().at(i);


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

}
