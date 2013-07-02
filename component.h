#ifndef COMPONENT_H
#define COMPONENT_H
#include <memory>
class Component
{
public:
    Component();
    virtual time_t nextCheckTime()=0;
};

class ComponentException : public std::exception
{
    const char *_what;
public:
    ComponentException(const char *_what);
    virtual const char *what();
};

Component *loadComponent(const char *path);
#endif // COMPONENT_H
