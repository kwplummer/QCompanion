#include "waitercrondialog.h"
#include "ui_waitercrondialog.h"

WaiterCronDialog::WaiterCronDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WaiterCronDialog) {
  ui->setupUi(this);
}

WaiterCronDialog::~WaiterCronDialog() { delete ui; }

WaiterCronOccurance WaiterCronDialog::getResult() {
  return WaiterCronOccurance{
    static_cast<short>(ui->minBox->value()),
    static_cast<short>(ui->hourBox->value()),
    static_cast<short>(ui->dayMonthBox->value()),
    static_cast<short>(ui->monthBox->value()),
    static_cast<short>(ui->dayWeekBox->currentIndex())
  };
}
