#include "mock.h"

time_t MockComponent::nextCheckTime()
{
    return 5;
}

void MockComponent::speak(Speaker &s)
{
    s.speak("this is a test.");
}

extern "C"
{
    Component *makeComponent()
    {
        return new MockComponent;
    }
}


