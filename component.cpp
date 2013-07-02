#include "component.h"
#include <dlfcn.h>
#include <functional>
Component::Component()
{
}

ComponentException::ComponentException(const char *_what)
    :_what(_what)
{
}

const char *ComponentException::what()
{
    return _what;
}


Component *loadComponent(const char *path)
{
    const auto handle = dlopen(path,RTLD_NOW);
    if(!handle)
    {
        throw ComponentException(dlerror());
    }
    typedef Component *(*fptr)();
    const auto makeFunction = reinterpret_cast<fptr>(dlsym(handle,"makeComponent"));
    if(!makeFunction)
    {
        throw ComponentException(dlerror());
    }
    return makeFunction();
}
