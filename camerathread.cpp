#include "camerathread.h"

CameraThread::CameraThread(QObject *parent) : QThread(parent)
{

}

void CameraThread::run()
{
    m_controller.addListener(m_listener);

    while(!isInterruptionRequested())
    {
        CameraStatus("Camera is connected");
        if(!m_controller.isConnected())
        {
            CameraStatus("Camera is disconnected... \nreconnect camera and cilick on turn on button again");
            return;
        }
        Leap::Hand rightHand = m_controller.frame().hands().rightmost();
        Leap::Hand leftHand  = m_controller.frame().hands().leftmost();
        Leap::Vector positionRight = rightHand.palmPosition();
        Leap::Vector positionLeft = leftHand.palmPosition();
        if(rightHand.isRight())
            CoordinatesRight(QString("x: %1 y: %2 z: %3 deg: %4").arg(positionRight.x).arg(positionRight.y).arg(positionRight.z).arg((rightHand.grabAngle()*180)/3.14));
        else
            CoordinatesRight(QString(""));
        if(leftHand.isLeft())
            CoordinatesLeft(QString("x: %1 y: %2 z: %3 deg: %4").arg(positionLeft.x).arg(positionLeft.y).arg(positionLeft.z).arg((leftHand.grabAngle()*180)/3.14));
        else
            CoordinatesLeft(QString(""));
    }
    CameraStatus("Camera is disconnected");
    m_controller.removeListener(m_listener);
}
