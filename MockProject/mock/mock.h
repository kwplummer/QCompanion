#ifndef MOCK_H
#define MOCK_H
#include "mock_global.h"
#include "../../component.h"
class MockComponent : public Component
{
public:
    virtual ~MockComponent(){}
    virtual time_t nextCheckTime() override;
    virtual std::string getText() override;
};

#endif // MOCK_H
