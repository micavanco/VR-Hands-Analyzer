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

    void on_recordButton_pressed();
    void onTimerTimeout();

private:
    Ui::MainWindow *ui;
    CameraThread   m_cameraThread;
    QTimer *timer;
    double time;
    double totalTime;
    int timeCount;
    QFile *file;
    QTextStream *streamOut;
    QString fileTitle;
};

#endif // MAINWINDOW_H
