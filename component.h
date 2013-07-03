#ifndef COMPONENT_H
#define COMPONENT_H
#include <exception>
#include <string>

//The base class from which all components extend from.
class Component
{
public:
    Component();
    virtual ~Component(){}
    //Note: While nextCheckTime() is not const (you can change state), it may be called many times before speak() is called.
    //You should not do something like increment and return a time by 60 seconds, as you'll never get called.
    //However, the last value returned is always checked before each call of nextCheckTime().
    virtual time_t nextCheckTime()=0;
    virtual std::string getText()=0;
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
