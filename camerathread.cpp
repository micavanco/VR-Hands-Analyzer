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
        Leap::Hand leftHand = m_controller.frame().hands().leftmost();
        Leap::Vector positionRight = rightHand.palmPosition();
        Leap::Vector positionLeft = leftHand.palmPosition();
        Coordinates(QString("x: %1 y: %2 z: %3").arg(position.x).arg(position.y).arg(position.z));
    }
    CameraStatus("Camera is disconnected");
    m_controller.removeListener(m_listener);
}
