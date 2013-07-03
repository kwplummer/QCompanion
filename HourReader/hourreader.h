#ifndef HOURREADER_H
#define HOURREADER_H

#include "hourreader_global.h"
#include "../component.h"

class HourReader : public Component
{
public:
    virtual ~HourReader(){}
    virtual time_t nextCheckTime() override;
    virtual std::string getText() override;
};

#endif // HOURREADER_H
