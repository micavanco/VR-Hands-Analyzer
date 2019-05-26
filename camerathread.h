#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include "Leap.h"
#include "samplelistener.h"
using namespace Leap;

class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent=nullptr);

signals:
    void CameraStatus(QString) const ;
    void CoordinatesRight(QString) const ;
    void CoordinatesLeft(QString) const ;

public slots:
    void run() override;
private:
    SampleListener m_listener;
    Controller m_controller;
};

#endif // CAMERATHREAD_H
