#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_cameraThread, SIGNAL(CameraStatus(QString)), ui->cameraStatusLabel, SLOT(setText(QString)));
    connect(&m_cameraThread, SIGNAL(Coordinates(QString)), ui->coordinatesLabel, SLOT(setText(QString)));
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
