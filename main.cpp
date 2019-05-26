#include "mainwindow.h"
#include <QApplication>
#include <QtDataVisualization>


using namespace QtDataVisualization;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    /*
    Q3DScatter scatter;
        scatter.setFlags(scatter.flags() ^ Qt::FramelessWindowHint);
        QScatter3DSeries *series = new QScatter3DSeries;
        QScatterDataArray data;
        data << QVector3D(0.5f, 0.5f, 0.5f) << QVector3D(-0.3f, -0.5f, -0.4f) << QVector3D(0.0f, -0.3f, 0.2f);
        series->dataProxy()->addItems(data);
        scatter.addSeries(series);
        scatter.show();
       */

    QFile stylesheet_file(":/Styles/resources/stylesheet.css");
    stylesheet_file.open(QFile::ReadOnly);
    QString stylesheet = QLatin1String(stylesheet_file.readAll());
    a.setStyleSheet(stylesheet);


    return a.exec();
}
