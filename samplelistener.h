#ifndef SAMPLELISTENER_H
#define SAMPLELISTENER_H

#include "Leap.h"
#include <QString>

using namespace Leap;

class SampleListener : public Listener
{ 
    public:
    virtual void onFrame(const Controller&);
private:

};

#endif // SAMPLELISTENER_H

