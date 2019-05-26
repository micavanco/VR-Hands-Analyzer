#ifndef SAMPLELISTENER_H
#define SAMPLELISTENER_H

#include "Leap.h"

using namespace Leap;

class SampleListener : public Listener
{
public:
virtual void onConnect(const Controller&);
virtual void onFrame(const Controller&);
};

#endif // SAMPLELISTENER_H

