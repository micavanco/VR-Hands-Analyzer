#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Leap.h"
#include <QtDataVisualization>
#include "samplelistener.h"

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
