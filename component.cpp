#include <QAction>
#include <QSettings>
#include "component.h"

Component::Component(QWidget *parent)  : QWidget(parent), muted(false)
{
    muteAction = new QAction("Mute",this);
    muteAction->setCheckable(true);
    connect(muteAction,SIGNAL(triggered(bool)),this,SLOT(setMute(bool)));
}

void Component::setMute(bool mute)
{
    muted = mute;
}

QList<QAction *> Component::getMenuContents()
{
    QList<QAction *> actions;
    actions.append(muteAction);
    return actions;
}

bool Component::isMuted()
{
    return muted;
}


