#include "waitercrondialog.h"
#include "ui_waitercrondialog.h"

WaiterCronDialog::WaiterCronDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WaiterCronDialog), ok(false)
{
  ui->setupUi(this);
}

WaiterCronDialog::~WaiterCronDialog()
{
  delete ui;
}

WaiterCronOccurance WaiterCronDialog::getResult()
{
  return WaiterCronOccurance{
    static_cast<short>(ui->minBox->value()),
    static_cast<short>(ui->hourBox->value()),
    static_cast<short>(ui->dayMonthBox->value()),
    static_cast<short>(ui->monthBox->value()),
    static_cast<short>(ui->dayWeekBox->currentIndex())
  };
}

bool WaiterCronDialog::okWasPressed()
{
  return ok;
}

void WaiterCronDialog::setResult(const WaiterCronOccurance &when)
{
  ui->minBox->setValue(when.repeatMin);
  ui->hourBox->setValue(when.repeatHour);
  ui->dayMonthBox->setValue(when.repeatDayOfMonth);
  ui->monthBox->setValue(when.repeatMonth);
  ui->dayWeekBox->setCurrentIndex(when.repeatDayOfWeek);
}

void WaiterCronDialog::on_addButton_clicked()
{
  ok = true;
  close();
}
