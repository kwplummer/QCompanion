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
    : QDialog(parent), ui(new Ui::WaiterDialog)
{
  ui->setupUi(this);
  ui->EntryDate->setSelectedDate(QDate::currentDate());
  currentTimeLabel = new QLabel(this);
  currentTimeLabel->setAlignment(Qt::AlignHCenter);
  ui->TimerScrollerWidget->layout()->addWidget(currentTimeLabel);
  QVariant saveLocation = settings.value("QWaiter_Location");
  if(!saveLocation.isNull())
  {
    statePath = saveLocation.toString();
  }
  loadState();
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(notifyAll()));
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
 * \param event used to tell the program not to close the window.
 */
void WaiterDialog::closeEvent(QCloseEvent *event)
{
  setVisible(false);
  event->ignore();
}

/*!
 * \brief Sets where the timers should be saved to
 * \param path Where the timers should be saved to
 */
void WaiterDialog::setStatePath(QString path)
{
  statePath = path;
  settings.setValue("QWaiter_Location", path);
  if(widgets.empty())
    loadState();
  else
    commitChanges();
}

/*!
 * \brief Updates when to speak next, then sends the text to the component.
 * \param what What to say.
 */
void WaiterDialog::emitTextSlot(QString what)
{
  Q_EMIT emitText(what);
  updateNextTime();
}

/*!
 * \brief Calls addWaiter with an empty WaiterCronOccurance
 */
void WaiterDialog::onAddButtonClicked()
{
  addWaiter(WaiterCronOccurance{});
}

/*!
 * \brief Called when "Add Recurring" is clicked.
 * \details Presents a WaiterCronDialog asking when repetitions should occur.
 */
void WaiterDialog::on_AddRecurringButton_clicked()
{
  WaiterCronDialog dialog;
  dialog.setResult(lastUsedRepeat);
  dialog.exec();
  if(dialog.okWasPressed())
  {
    lastUsedRepeat = dialog.getResult();
    addWaiter(dialog.getResult());
  }
}
/*!
 * \brief Adds and configures a "Waiter" that waits and notifies about an event.
 * \param repeat When repetition occurs. Can be empty.
 */
void WaiterDialog::addWaiter(const WaiterCronOccurance &repeat)
{
  WaiterWidget *w =
      new WaiterWidget(this, ui->EntryDate->selectedDate(),
                       ui->EntryTime->time(), ui->EventTitle->text(), repeat);
  widgets.push_back(w);
  ui->TimerScrollerWidget->layout()->addWidget(w);
  connect(w, SIGNAL(removeAt(WaiterWidget *)), this,
          SLOT(removeWaiter(WaiterWidget *)));
  connect(w, SIGNAL(replaceAt(WaiterWidget *, QString, QDateTime,
                              WaiterCronOccurance)),
          this, SLOT(replaceWaiter(WaiterWidget *, QString, QDateTime,
                                   WaiterCronOccurance)));
  connect(w, SIGNAL(speakThis(QString)), this, SLOT(emitTextSlot(QString)));
  connect(w, SIGNAL(repeatAt(WaiterWidget *, QString, QDateTime,
                             WaiterCronOccurance)),
          this, SLOT(repeatWaiter(WaiterWidget *, QString, QDateTime,
                                  WaiterCronOccurance)));
  ui->EventTitle->clear();
  commitChanges();
}

/*!
 * \brief Updates all the Waiters with the current time, additionally sets the
 * currentTimeLabel to the current time.
 */
void WaiterDialog::notifyAll()
{
  setUpdatesEnabled(false);
  const QDateTime current = QDateTime::currentDateTime();
  currentTimeLabel->setText(current.toString("yy/MM/dd hh:mm:ss"));
  for(WaiterWidget *w : widgets)
    w->update(current);
  setUpdatesEnabled(true);
}

/*!
 * \brief Sets the two fields used to set a timer's date and time to the current
 * date and time.
 */
void WaiterDialog::onCurrentTimeClicked()
{
  ui->EntryDate->setSelectedDate(QDate::currentDate());
  ui->EntryTime->setTime(QTime::currentTime());
}

/*!
 * \brief Removes a specified Waiter.
 * \param waiter what to remove.
 */
void WaiterDialog::removeWaiter(WaiterWidget *waiter)
{
  widgets.erase(std::remove(widgets.begin(), widgets.end(), waiter),
                widgets.end());
  waiter->deleteLater();
  commitChanges();
}

/*!
 * \brief Removes a waiter, and sets all the entry fields to what were used to
 * create it.
 * \param waiter What to remove
 * \param name What the old waiter was called
 * \param datetime When the waiter was waiting until
 * \param repeat The time when the waiter will repeat. Can be empty.
 */
void WaiterDialog::replaceWaiter(WaiterWidget *waiter, QString name,
                                 QDateTime datetime,
                                 const WaiterCronOccurance &repeat)
{
  ui->EventTitle->setText(name);
  ui->EntryDate->setSelectedDate(datetime.date());
  ui->EntryTime->setTime(datetime.time());
  lastUsedRepeat = repeat;
  widgets.erase(std::remove(widgets.begin(), widgets.end(), waiter),
                widgets.end());
  waiter->deleteLater();
  commitChanges();
}

/*!
 * \brief Readds a Waiter that is set to repeat.
 * \param waiter The waiter to remove, as it has been copied.
 * \param title The title of the waiter to readd
 * \param datetime The time of the waiter to readd.
 * \param repeat The time to repeat.
 */
void WaiterDialog::repeatWaiter(WaiterWidget *waiter, QString title,
                                QDateTime datetime,
                                const WaiterCronOccurance &repeat)
{
  const auto oldDate = ui->EntryDate->selectedDate();
  const auto oldTime = ui->EntryTime->time();
  const auto oldText = ui->EventTitle->text();
  ui->EntryDate->setSelectedDate(datetime.date());
  ui->EntryTime->setTime(datetime.time());
  ui->EventTitle->setText(title);

  addWaiter(repeat);

  ui->EntryDate->setSelectedDate(oldDate);
  ui->EntryTime->setTime(oldTime);
  ui->EventTitle->setText(oldText);

  removeWaiter(waiter);
}

/*!
 * \brief Saves all the waiters to disk.
 * \details Saves all the waiters to disk. Called whenever a waiter is added or
 * removed.
 * Additionally this updates the time the next speech should occur, and notifies
 * the component.
 */
void WaiterDialog::commitChanges()
{
  if(!statePath.isNull())
  {
    std::sort(widgets.begin(), widgets.end(),
              [](const WaiterWidget *w1, const WaiterWidget *w2)
              { return w1->getMsecs() < w2->getMsecs(); });
    for(auto w : widgets)
      ui->TimerScrollerWidget->layout()->removeWidget(w);
    for(auto w : widgets)
      ui->TimerScrollerWidget->layout()->addWidget(w);
    QFile file(statePath);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    for(WaiterWidget *w : widgets)
    {
      out << w->toLoggableString() << '\n';
    }
    updateNextTime();
    Q_EMIT changeTimers();
  }
}

/*!
 * \brief Loads all the waiters from disk.
 * \details Loads all the waiters from disk. Called on startup or when the save
 * path changes.
 * Additionally this updates the time the next speech should occur, and notifies
 * the component.
 */
void WaiterDialog::loadState()
{
  if(!statePath.isNull())
  {
    QFile file(statePath);
    if(file.exists())
    {
      file.open(QIODevice::ReadOnly);
      QTextStream in(&file);
      // Use in.readLine() instead? This is kind of a dumb way to do it...
      while(!in.atEnd())
      {
        QStringList line = in.readLine().split(' ');

        if(line.size() < 9)
          return;

        QString title = line[0];
        qint64 time = line[1].toLongLong();
        WaiterCronOccurance repeat{};
        QList<short> days;
        for(QString s : line.mid(9))
        {
          days.append(s.toShort());
        }
        if(line.size() >= 7)
        {
          repeat = WaiterCronOccurance(line[2].toShort(), line[3].toShort(),
                                       line[4].toShort(), line[5].toShort(),
                                       line[6].toLong(), line[7].toLong(),
                                       line[8].toInt(), days);
        }

        if(!title.isEmpty())
        {
          WaiterWidget *w = new WaiterWidget(
              this, time, title.replace("_", " ", Qt::CaseInsensitive), repeat);
          widgets.push_back(w);
          ui->TimerScrollerWidget->layout()->addWidget(w);
          connect(w, SIGNAL(removeAt(WaiterWidget *)), this,
                  SLOT(removeWaiter(WaiterWidget *)));
          connect(w, SIGNAL(replaceAt(WaiterWidget *, QString, QDateTime,
                                      WaiterCronOccurance)),
                  this, SLOT(replaceWaiter(WaiterWidget *, QString, QDateTime,
                                           WaiterCronOccurance)));
          connect(w, SIGNAL(speakThis(QString)), this,
                  SLOT(emitTextSlot(QString)));
          connect(w, SIGNAL(repeatAt(WaiterWidget *, QString, QDateTime,
                                     WaiterCronOccurance)),
                  this, SLOT(repeatWaiter(WaiterWidget *, QString, QDateTime,
                                          WaiterCronOccurance)));
        }
      }
    }
    updateNextTime();
    Q_EMIT changeTimers();
  }
}

/*!
 * \brief Sets when the next speech will occur.
 * \details Takes the current time and every time
 */
void WaiterDialog::updateNextTime()
{
  const time_t now = QDateTime::currentDateTime().toMSecsSinceEpoch();
  time_t earliest = 0;
  const std::vector<time_t> times{31536000000, 2592000000, 604800000, 86400000,
                                  43200000,    21600000,   21600000,  3600000,
                                  1800000,     900000,     600000,    300000,
                                  60000,       0};
  for(const WaiterWidget *w : widgets)
  {
    const time_t diff = w->getMsecs() - now;
    for(time_t t : times)
    {
      if(diff > t)
      {
        if(!earliest || earliest > diff - t)
        {
          earliest = diff - t;
        }
      }
    }
  }
  if(earliest)
    nextTime = QDateTime::currentDateTime().addMSecs(earliest);
  else
    nextTime = QDateTime::fromTime_t(0);
}
