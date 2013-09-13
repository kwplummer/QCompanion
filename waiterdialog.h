#ifndef WAITERDIALOG_H
#define WAITERDIALOG_H
#include <QDialog>
#include <QSettings>
#include "waiterwidget.h"
#include "waitercrondialog.h"
namespace Ui {
class WaiterDialog;
}

/*!
 * \brief The GUI used to configure waiters.
 */
class WaiterDialog : public QDialog {
  Q_OBJECT
  void updateNextTime();
  void commitChanges();
  void loadState();
  void addWaiter(const WaiterCronOccurance &repeat);
  ///\brief A pointer to the UI elements.
  Ui::WaiterDialog *ui;
  ///\brief A container of all the Waiters.
  std::vector<WaiterWidget *> widgets;
  ///\brief When the next speech should occur.
  QDateTime nextTime;
  ///\brief A timer used to update the UI every second.
  QTimer *timer;
  ///\brief Where the waiters are saved to
  QString statePath;
  ///\brief A bit of text saying the current time
  QLabel *currentTimeLabel;
  ///\brief The settings of the program, used to check/save where StatePath is.
  QSettings settings;
  WaiterCronOccurance lastUsedRepeat;
private
slots:
  void onAddButtonClicked();
  void notifyAll();
  void onCurrentTimeClicked();
  void removeWaiter(WaiterWidget *waiter);
  void replaceWaiter(WaiterWidget *waiter, QString name, QDateTime datetime);
  void emitTextSlot(QString what);

  void on_AddRecurringButton_clicked();

protected:
  void closeEvent(QCloseEvent *event) override;
signals:
  ///\brief Emitted to send text to the Component.
  void emitText(QString);
  /*!
   *\brief Tells the rest of the program that the countdown for next speech
   *should be recalculated.
   */
  void changeTimers();

public:
  void setStatePath(QString path);
  explicit WaiterDialog(QWidget *parent, QString IconPath);
  QDateTime getNextTime();
  ~WaiterDialog();
};

#endif // WAITERDIALOG_H
