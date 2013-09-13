#ifndef WAITERCRONDIALOG_H
#define WAITERCRONDIALOG_H

#include <QDialog>
#include "waitercronoccurance.h"
namespace Ui
{
class WaiterCronDialog;
}

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
  Ui::WaiterCronDialog *ui;
  bool ok;
};

#endif // WAITERCRONDIALOG_H
