#include "mock.h"

time_t MockComponent::nextCheckTime()
{
    return 5;
}

std::string MockComponent::getText()
{
    return "this is a test.";
}

extern "C"
{
    Component *makeComponent()
    {
        return new MockComponent;
    }
}


