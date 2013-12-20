#ifndef WAITERCOMPONENT_H
#define WAITERCOMPONENT_H
#include "component.h"
#include "waiterdialog.h"

/*!
 * \brief A componet that provides reminders for scheduled events.
 * \details provides reminders for scheduled events, as well as a GUI to view
 * and modify the events.
 * Notifications are provided one year, one month, one week, one day, 12 hours,
 * 6 hours, one hour, 30 minutes, 15 minutes, 10 minutes, 5 minutes, and one
 * minute before the event. As well as when the event occurs.
 */
class WaiterComponent : public Component
{
  Q_OBJECT
  ///\brief The GUI that handles creating, deleting, and updating timers.
  WaiterDialog dialog;
private Q_SLOTS:
  void emitSpeak(QString what);
public Q_SLOTS:
  void selectStatePath();
  void changeTimerSlot();
Q_SIGNALS:
  /*!
   * \brief Tells the rest of the program that the countdown for next speech
   * should be recalculated.
   */
  void changeTimers();

public:
  explicit WaiterComponent(QWidget *parent = 0);
  virtual ~WaiterComponent();
  virtual QDateTime nextCheckTime() override;
  virtual QList<QAction *> getMenuContents() override;
};

#endif // WAITERCOMPONENT_H
