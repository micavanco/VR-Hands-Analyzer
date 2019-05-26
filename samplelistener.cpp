
#include "samplelistener.h"


void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
    std::cout << "Frame available" << std::endl;
}

