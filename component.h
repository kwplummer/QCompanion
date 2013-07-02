#ifndef COMPONENT_H
#define COMPONENT_H
#include <time.h>
#include <exception>
#include "speaker.h"

//The base class from which all components extend from.
class Component
{
public:
    Component();
    virtual ~Component(){}
    virtual time_t nextCheckTime()=0;
    virtual void speak(Speaker &s)=0;
};

//An exception thrown when a component cannot be dynamically loaded.
class ComponentException : public std::exception
{
    const char *_what;
public:
    ComponentException(const char *_what);
    virtual const char *what();
};

//Loads the component using dlopen()
Component *loadComponent(const char *path);
#endif // COMPONENT_H
