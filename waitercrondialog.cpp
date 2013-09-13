#include "waitercrondialog.h"
#include "ui_waitercrondialog.h"

WaiterCronDialog::WaiterCronDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WaiterCronDialog) {
  ui->setupUi(this);
}

WaiterCronDialog::~WaiterCronDialog() { delete ui; }
