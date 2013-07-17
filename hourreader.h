#ifndef HOURREADER_H
#define HOURREADER_H
#include "component.h"
class HourReader : public Component
{
    Q_OBJECT
public:
    HourReader(QWidget *parent) : Component(parent) {}
    virtual ~HourReader(){}
    virtual QDateTime nextCheckTime() override;
    virtual QString getText() override;
};

#endif // HOURREADER_H
