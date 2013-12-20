#ifndef WAITERCRONDIALOG_H
#define WAITERCRONDIALOG_H

#include <QDialog>
#include "waitercronoccurance.h"
namespace Ui
{
class WaiterCronDialog;
}
/*!
 * \brief A dialog used for asking a user when they want an event to repeat.
 * \details There are two tabs in the dialog, one for each of
 * WaiterCronOccurance's two modes. The first tab is used to enter in a delay. A
 * "1" in the hour field would mean that the event repeats every hour, and so
 * when it finishes it will be readded with a new end date that is 60 minutes
 * later. All of the inputs are summed up when creating the delay.\n\n The other
 * tab is for specifying the time at which an event will repeat. A "1" in the
 * hour field in this mode would mean every day at one. Chaining multiple fields
 * restricts how often the event repeats, in a manner similar to cron (but not
 * as featureful). In this mode "1" in the hour field, and "30" in the minutes
 * field, with "Wednesday" selected on the list of days would mean "Wednesday at
 * 1:30 am." 24 hour time is used.
 */
class WaiterCronDialog : public QDialog
{
  Q_OBJECT

public:
  explicit WaiterCronDialog(QWidget *parent = 0);
  ~WaiterCronDialog();
  WaiterCronOccurance getResult();
  bool okWasPressed();
  void setResult(const WaiterCronOccurance &when);
public Q_SLOTS:
  void changeTab(int which);
private Q_SLOTS:
  void on_addButton_clicked();

private:
  ///\brief A pointer to the UI elements.
  Ui::WaiterCronDialog *ui;
  ///\brief A bool representing if the UI was closed via the OK button.
  bool ok;
};

#endif // WAITERCRONDIALOG_H
