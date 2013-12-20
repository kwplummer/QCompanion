#include <QCoreApplication>
#include <QAction>
#include <QFileDialog>
#include "waitercomponent.h"

/*!
 * \brief Constructs the component and its dialog.
 * \details Constructs the component and its dialog, also connects Q_SIGNALS
 * used
 * to notify when timers have changed.
 * \param parent
 */
WaiterComponent::WaiterComponent(QWidget *parent)
    : Component(parent),
      dialog(parent, QCoreApplication::applicationDirPath() + "/murasaki.png")
{
  dialog.setWindowFlags(Qt::Window);
  connect(&dialog, SIGNAL(changeTimers()), this, SLOT(changeTimerSlot()));
  connect(&dialog, SIGNAL(Q_EMITText(QString)), this, SLOT(emitSpeak(QString)));
}

/*!
 * \brief Destroys the component and dialog.
 */
WaiterComponent::~WaiterComponent() {}

/*!
 * \brief Asks the dialog when it will speak next.
 * \return The QDateTime when it will speak next.
 */
QDateTime WaiterComponent::nextCheckTime() { return dialog.getNextTime(); }

/*!
 * \brief Creates a specialized menu for the component.
 * \details Creates a menu that in addition to muting, allows you to choose
 * where to save your timers, and show the dialog to configure timers.
 * \return A QList of QActions that can be used to interface with the component.
 */
QList<QAction *> WaiterComponent::getMenuContents()
{
  QList<QAction *> returnMe;
  QAction *showAction = new QAction("Show", this);
  connect(showAction, SIGNAL(triggered()), &dialog, SLOT(show()));
  returnMe.append(showAction);

  QAction *changeStatePath = new QAction("Change State Location", this);
  connect(changeStatePath, SIGNAL(triggered()), this, SLOT(selectStatePath()));
  returnMe.append(changeStatePath);

  returnMe += Component::getMenuContents();
  return returnMe;
}

/*!
 * \brief Asks the user where they want to store the state file used for the
 * timers.
 */
void WaiterComponent::selectStatePath()
{
  QFileDialog dlg(this, "Select Folder");
  QString dir = dlg.getOpenFileName(this, "Open Directory", "~/");
  if(!dir.isNull())
  {
    dialog.setStatePath(dir);
  }
}

/*!
 * \brief Emitted when the timers are modified.
 * \details When the user changes a timer this is used to notify the rest of the
 * program that the timers have changed.
 */
void WaiterComponent::changeTimerSlot() { Q_EMIT changeTimers(); }

/*!
 * \brief Checks if it's muted, and if not sends the text up to the QCompanion.
 * \param what What to say
 */
void WaiterComponent::emitSpeak(QString what)
{
  if(!muted)
    Q_EMIT wantsToSpeak(what);
  Q_EMIT changeTimers();
}
