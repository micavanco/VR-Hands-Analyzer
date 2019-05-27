#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDataVisualization>
#include <QFileDialog>

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

    void on_loadPatientFileButton_pressed();

    void on_deletePatientFileButton_pressed();

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
    QMap<QString, QVector<double>>  m_filePatientStorageTime;
    QMap<QString, QVector<double>> m_filePatientStorageLeftX;
    QMap<QString, QVector<double>> m_filePatientStorageLeftY;
    QMap<QString, QVector<double>> m_filePatientStorageLeftZ;
    QMap<QString, QVector<double>> m_filePatientStorageLeftDeg;
    QMap<QString, QVector<double>> m_filePatientStorageRightX;
    QMap<QString, QVector<double>> m_filePatientStorageRightY;
    QMap<QString, QVector<double>> m_filePatientStorageRightZ;
    QMap<QString, QVector<double>> m_filePatientStorageRightDeg;
    QMap<QString, int>     m_filePatientStorageLength;
};

#endif // MAINWINDOW_H
