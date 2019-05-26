#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_turnOnButton_pressed()
{
    using namespace Leap;

    SampleListener listener;
    Controller controller;

    controller.addListener(listener);

        // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

        // Remove the sample listener when done
    controller.removeListener(listener);
}
