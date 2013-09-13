#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QMenu>
#include <QCloseEvent>
#include <algorithm>
#include "waiterdialog.h"
#include "ui_waiterdialog.h"

/*!
 * \brief Constructs the WaiterDialog.
 * \details Constructs the WaiterDialog, loads timers and starts updating
 * timers.
 * \param parent The widget's parent, used for Qt's memory management.
 * \param IconPath Where the icon for the GUI is located.
 */
WaiterDialog::WaiterDialog(QWidget *parent, QString IconPath)
    : QDialog(parent), ui(new Ui::WaiterDialog), lastUsedRepeat{} {
  ui->setupUi(this);
  ui->EntryDate->setSelectedDate(QDate::currentDate());
  currentTimeLabel = new QLabel(this);
  currentTimeLabel->setAlignment(Qt::AlignHCenter);
  ui->TimerScrollerWidget->layout()->addWidget(currentTimeLabel);
  QVariant saveLocation = settings.value("QWaiter_Location");
  if (!saveLocation.isNull()) {
    statePath = saveLocation.toString();
  }
  loadState();
  timer = new QTimer(this);
#if QT_VERSION < 0x050000
  connect(timer, SIGNAL(timeout()), this, SLOT(notifyAll()));
#else
  connect(timer, &QTimer::timeout, this, &WaiterDialog::notifyAll);
#endif
  timer->setInterval(1000);
  timer->start();
  setWindowIcon(QIcon(IconPath));
}

/*!
 * \brief Gets the next time there should be text.
 * \return A QDateTime of when the next speech should occur.
 */
QDateTime WaiterDialog::getNextTime() { return nextTime; }

/*!
 * \brief Deletes the UI.
 */
WaiterDialog::~WaiterDialog() { delete ui; }

/*!
 * \brief Hides the UI, rather than actually closing it.
 * \param[in] event used to tell the program not to close the window.
 */
void WaiterDialog::closeEvent(QCloseEvent *event) {
  setVisible(false);
  event->ignore();
}

/*!
 * \brief Sets where the timers should be saved to
 * \param[in] path Where the timers should be saved to
 */
void WaiterDialog::setStatePath(QString path) {
  statePath = path;
  settings.setValue("QWaiter_Location", path);
  if (widgets.empty())
    loadState();
  else
    commitChanges();
}

/*!
 * \brief Updates when to speak next, then sends the text to the component.
 * \param[in] what What to say.
 */
void WaiterDialog::emitTextSlot(QString what) {
  emit emitText(what);
  updateNextTime();
}

/*!
 * \brief Adds and configures a "Waiter" that waits and notifies about an event.
 */
void WaiterDialog::onAddButtonClicked() {
  addWaiter(WaiterCronOccurance{});
}

/*!
 * \brief Updates all the Waiters with the current time, additionally sets the
 * currentTimeLabel to the current time.
 */
void WaiterDialog::notifyAll() {
  const QDateTime current = QDateTime::currentDateTime();
  currentTimeLabel->setText(current.toString("yy/MM/dd hh:mm:ss"));
  for (WaiterWidget *w : widgets)
    w->update(current);
}

/*!
 * \brief Sets the two fields used to set a timer's date and time to the current
 * date and time.
 */
void WaiterDialog::onCurrentTimeClicked() {
  ui->EntryDate->setSelectedDate(QDate::currentDate());
  ui->EntryTime->setTime(QTime::currentTime());
}

/*!
 * \brief Removes a specified Waiter.
 * \param[in] waiter what to remove.
 */
void WaiterDialog::removeWaiter(WaiterWidget *waiter) {
  widgets.erase(std::remove(widgets.begin(), widgets.end(), waiter),
                widgets.end());
  waiter->deleteLater();
  commitChanges();
}

/*!
 * \brief Removes a waiter, and sets all the entry fields to what were used to
 * create it.
 * \param[in] waiter What to remove
 * \param[in] name What the old waiter was called
 * \param[in] datetime When the waiter was waiting until
 */
void WaiterDialog::replaceWaiter(WaiterWidget *waiter, QString name,
                                 QDateTime datetime) {
  ui->EventTitle->setText(name);
  ui->EntryDate->setSelectedDate(datetime.date());
  ui->EntryTime->setTime(datetime.time());
  widgets.erase(std::remove(widgets.begin(), widgets.end(), waiter),
                widgets.end());
  waiter->deleteLater();
  commitChanges();
}

/*!
 * \brief Saves all the waiters to disk.
 * \details Saves all the waiters to disk. Called whenever a waiter is added or
 * removed.
 * Additionally this updates the time the next speech should occur, and notifies
 * the component.
 */
void WaiterDialog::commitChanges() {
  if (!statePath.isNull()) {
    std::sort(widgets.begin(), widgets.end(),
              [](const WaiterWidget * w1, const WaiterWidget * w2) {
      return w1->getMsecs() < w2->getMsecs();
    });
    for (auto w : widgets)
      ui->TimerScrollerWidget->layout()->removeWidget(w);
    for (auto w : widgets)
      ui->TimerScrollerWidget->layout()->addWidget(w);
    QFile file(statePath);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    for (WaiterWidget *w : widgets) {
      out << w->toLoggableString() << '\n';
    }
    updateNextTime();
    emit changeTimers();
  }
}

/*!
 * \brief Loads all the waiters from disk.
 * \details Loads all the waiters from disk. Called on startup or when the save
 * path changes.
 * Additionally this updates the time the next speech should occur, and notifies
 * the component.
 */
void WaiterDialog::loadState() {
  if (!statePath.isNull()) {
    QFile file(statePath);
    if (file.exists()) {
      file.open(QIODevice::ReadOnly);
      QTextStream in(&file);
      while (!in.atEnd()) {
        QString title;
        qint64 time;
        in >> title >> time;
        if (!title.isEmpty()) {
          WaiterWidget *w = new WaiterWidget(
              this, time, title.replace("_", " ", Qt::CaseInsensitive));
          widgets.push_back(w);
          ui->TimerScrollerWidget->layout()->addWidget(w);
#if QT_VERSION < 0x050000
          connect(w, SIGNAL(removeAt(WaiterWidget *)), this,
                  SLOT(removeWaiter(WaiterWidget *)));
          connect(w, SIGNAL(replaceAt(WaiterWidget *, QString, QDateTime)),
                  this,
                  SLOT(replaceWaiter(WaiterWidget *, QString, QDateTime)));
          connect(w, SIGNAL(speakThis(QString)), this,
                  SLOT(emitTextSlot(QString)));
#else
          connect(w, &WaiterWidget::removeAt, this,
                  &WaiterDialog::removeWaiter);
          connect(w, &WaiterWidget::replaceAt, this,
                  &WaiterDialog::replaceWaiter);
          connect(w, &WaiterWidget::speakThis, this,
                  &WaiterDialog::emitTextSlot);
#endif
        }
      }
    }
    updateNextTime();
    emit changeTimers();
  }
}

void WaiterDialog::addWaiter(const WaiterCronOccurance &repeat) {
  WaiterWidget *w =
      new WaiterWidget(this, ui->EntryDate->selectedDate(),
                       ui->EntryTime->time(), ui->EventTitle->text());
  widgets.push_back(w);
  ui->TimerScrollerWidget->layout()->addWidget(w);
#if QT_VERSION < 0x050000
  connect(w, SIGNAL(removeAt(WaiterWidget *)), this,
          SLOT(removeWaiter(WaiterWidget *)));
  connect(w, SIGNAL(replaceAt(WaiterWidget *, QString, QDateTime)), this,
          SLOT(replaceWaiter(WaiterWidget *, QString, QDateTime)));
  connect(w, SIGNAL(speakThis(QString)), this, SLOT(emitTextSlot(QString)));
#else
  connect(w, &WaiterWidget::removeAt, this, &WaiterDialog::removeWaiter);
  connect(w, &WaiterWidget::replaceAt, this, &WaiterDialog::replaceWaiter);
  connect(w, &WaiterWidget::speakThis, this, &WaiterDialog::emitTextSlot);
#endif
  ui->EventTitle->clear();
  commitChanges();
}

/*!
 * \brief Sets when the next speech will occur.
 * \details Takes the current time and every time
 */
void WaiterDialog::updateNextTime() {
  const time_t now = QDateTime::currentDateTime().toMSecsSinceEpoch();
  time_t earliest = 0;
  const std::vector<time_t> times{ 31536000000, 2592000000, 604800000, 86400000,
                                   43200000,    21600000,   21600000,  3600000,
                                   1800000,     900000,     600000,    300000,
                                   60000,       0 };
  for (const WaiterWidget *w : widgets) {
    const time_t diff = w->getMsecs() - now;
    for (time_t t : times) {
      if (diff > t) {
        if (!earliest || earliest > diff - t) {
          earliest = diff - t;
        }
      }
    }
  }
  if (earliest)
    nextTime = QDateTime::currentDateTime().addMSecs(earliest);
  else
    nextTime = QDateTime::fromTime_t(0);
}

void WaiterDialog::on_AddRecurringButton_clicked() {
  WaiterCronDialog dialog;
  dialog.setResult(lastUsedRepeat);
  dialog.exec();
  if (dialog.okWasPressed()) {
    lastUsedRepeat = dialog.getResult();
    addWaiter(dialog.getResult());
  }
}
