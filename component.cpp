#include <QAction>
#include <QSettings>
#include "component.h"

/*!
 * \brief Creates and connects an action for muting.
 * \param parent The parent widget, used for Qt's parent/child memory management.
 */
Component::Component(QWidget *parent)  : QWidget(parent), muted(false)
{
    muteAction = new QAction("Mute",this);
    muteAction->setCheckable(true);
    connect(muteAction,SIGNAL(triggered(bool)),this,SLOT(setMute(bool)));
}

/*!
 * \brief Sets if the component should be in the list of components to speak and wait for.
 * \param[in] mute If the component should be muted.
 */
void Component::setMute(bool mute)
{
    muted = mute;
}

/*!
 * \brief Gets a list of menu items used for interacting with the component's features from the main menu.
 * \return The list to be added to the main menu.
 */
QList<QAction *> Component::getMenuContents()
{
    QList<QAction *> actions;
    actions.append(muteAction);
    return actions;
}

/*!
 * \brief Indicated if the component should have its message sent in a notification/speech
 * \return If the component is muted.
 */
bool Component::isMuted()
{
    return muted;
}


