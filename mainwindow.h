#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDataVisualization>

#include "camerathread.h"

using namespace QtDataVisualization;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_turnOnButton_pressed();

    void on_turnOffButton_pressed();

private:
    Ui::MainWindow *ui;
    CameraThread   m_cameraThread;
};

#endif // MAINWINDOW_H
