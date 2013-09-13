#ifndef WAITERCRONDIALOG_H
#define WAITERCRONDIALOG_H

#include <QDialog>
#include "waitercronoccurance.h"
namespace Ui {
class WaiterCronDialog;
}

class WaiterCronDialog : public QDialog {
  Q_OBJECT

public:
  explicit WaiterCronDialog(QWidget *parent = 0);
  ~WaiterCronDialog();
  WaiterCronOccurance getResult();

private:
  Ui::WaiterCronDialog *ui;
};

#endif // WAITERCRONDIALOG_H
