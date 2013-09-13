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
  return WaiterCronOccurance{
    static_cast<short>(ui->minBox->value()),
    static_cast<short>(ui->hourBox->value()),
    static_cast<short>(ui->dayMonthBox->value()),
    static_cast<short>(ui->monthBox->value()),
    static_cast<short>(ui->dayWeekBox->currentIndex())
  };
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
  ui->minBox->setValue(when.repeatMin);
  ui->hourBox->setValue(when.repeatHour);
  ui->dayMonthBox->setValue(when.repeatDayOfMonth);
  ui->monthBox->setValue(when.repeatMonth);
  ui->dayWeekBox->setCurrentIndex(when.repeatDayOfWeek);
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
