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
 * \details The form is similar to cron, however it currently lacks the ability
 * to use any syntax that is not * (-1 for this) or a number.
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

private
slots:
  void on_addButton_clicked();

private:
  ///\brief A pointer to the UI elements.
  Ui::WaiterCronDialog *ui;
  ///\brief A bool representing if the UI was closed via the OK button.
  bool ok;
};

#endif // WAITERCRONDIALOG_H
