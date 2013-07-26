#ifndef COMPONENT_H
#define COMPONENT_H
#include <Qt/qwidget.h>
#include <QDateTime>

//The base class from which all components extend from.
//Todo: Add QWaiter component, QSnappr component, Doxygen.
class Component : public QWidget
{
    Q_OBJECT
private slots:
    void setMute(bool mute);
protected:
    bool muted;
    QAction *muteAction;
public:
    Component(QWidget *parent);
    virtual ~Component(){}
    //Note: While nextCheckTime() is not const (you can change state), it may be called many times before speak() is called.
    //You should not do something like increment and return a time by 60 seconds, as you'll never get called.
    //However, the last value returned is always checked before each call of nextCheckTime().
    virtual QDateTime nextCheckTime()=0;
    virtual QString getText()=0;
    virtual QList<QAction*> getMenuContents();
    bool isMuted();
};
#endif // COMPONENT_H
