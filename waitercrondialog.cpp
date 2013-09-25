#include "waitercrondialog.h"
#include "ui_waitercrondialog.h"

/*!
 * \brief Constructs the dialog.
 * \param parent used for Qt's memory management.
 */
WaiterCronDialog::WaiterCronDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WaiterCronDialog), ok(false)
{
  ui->setupUi(this);
}
///\brief Deletes the UI.
WaiterCronDialog::~WaiterCronDialog()
{
  delete ui;
}
/*!
 * \brief Gets the data from the GUI.
 * \return A struct containing when to repeat.
 */
WaiterCronOccurance WaiterCronDialog::getResult()
{
  QList<short> days;
  for(const QModelIndex &i : ui->dayList->selectionModel()->selectedIndexes())
  {
    days.append(i.row() + 1);
  }
  if(ui->tabs->currentIndex() == 0)
  {
    return WaiterCronOccurance(
        ui->repeatMins->value(), ui->repeatHours->value(),
        ui->repeatDays->value(), ui->repeatWeeks->value(),
        ui->repeatMonths->value(), ui->repeatYears->value(), true, days);
  }
  return WaiterCronOccurance(ui->minBox->value(), ui->hourBox->value(),
                             ui->dayMonthBox->value(), 0, ui->monthBox->value(),
                             0, false, days);
}
/*!
 * \brief Returns if the UI was closed via the "OK" button.
 * \return A bool showing if the OK button was pressed.
 */
bool WaiterCronDialog::okWasPressed()
{
  return ok;
}
/*!
 * \brief Sets the boxes to a previously created WaiterCronOccurance
 * \param when What to set the boxes to.
 */
void WaiterCronDialog::setResult(const WaiterCronOccurance &when)
{
  if(when.isDelay)
  {
    ui->repeatMins->setValue(when.repeatMinutes);
    ui->repeatHours->setValue(when.repeatHour);
    ui->repeatDays->setValue(when.repeatDay);
    ui->repeatWeeks->setValue(when.repeatWeek);
    ui->repeatMonths->setValue(when.repeatMonth);
    ui->repeatYears->setValue(when.repeatYear);
    ui->tabs->setCurrentIndex(0);
  }
  else
  {
    ui->minBox->setValue(when.repeatMinutes);
    ui->hourBox->setValue(when.repeatHour);
    ui->dayMonthBox->setValue(when.repeatDay);
    ui->monthBox->setValue(when.repeatMonth);
    ui->dayList->clearSelection();
    ui->tabs->setCurrentIndex(1);
  }
  for(short s : when.repeatDays)
  {
    if(s - 1 >= 0 && s - 1 < ui->dayList->count())
      ui->dayList->item(s - 1)->setSelected(true);
  }
}
/*!
 * \brief Changes the tab to the requested position. Only used for unit tests.
 * \param which Which tab to change to.
 */
void WaiterCronDialog::changeTab(int which)
{
  ui->tabs->setCurrentIndex(which);
}
/*!
 * \brief Closes the window, and sets a bool stating the the UI was closed via
 * the OK button.
 */
void WaiterCronDialog::on_addButton_clicked()
{
  ok = true;
  close();
}
