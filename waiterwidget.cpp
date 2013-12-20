#include <QApplication>
#include <QBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include "waiterwidget.h"

/*!
 * \brief Constructs the Waiter Widget and calls init()
 * \param parent The widget's parent, used for Qt's memory management
 * \param endDate What date the waiter will wait for
 * \param endTime What time the waiter will wait for
 * \param title What the waiter is called
 * \param repeat When the waiter will repeat.
 */
WaiterWidget::WaiterWidget(QWidget *parent, QDate endDate, QTime endTime,
                           QString title, const WaiterCronOccurance &repeat)
    : QWidget(parent), datetime(endDate, endTime),
      title(title.replace("_", " ")),
      initMs(QDateTime::currentDateTime().toMSecsSinceEpoch()), quit(false),
      repeat(repeat)
{ init(); }

/*!
 * \brief Constructs the Waiter Widget and calls init()
 * \param parent The widget's parent, used for Qt's memory management
 * \param msec The miliseconds from the UNIX epoch
 * \param title What the waiter is called
 * \param repeat When the waiter will repeat.
 */
WaiterWidget::WaiterWidget(QWidget *parent, qint64 msec, QString title,
                           const WaiterCronOccurance &repeat)
    : QWidget(parent), datetime(QDateTime::fromMSecsSinceEpoch(msec)),
      title(title), initMs(QDateTime::currentDateTime().toMSecsSinceEpoch()),
      quit(false), repeat(repeat)
{ init(); }

/*!
 * \brief Creates all the UI elements and wires them up.
 */
void WaiterWidget::init()
{
  time = new QLabel(this);
  QString TimeText = "yy/MM/dd";
  const QTime TimerTime = datetime.time();
  if(TimerTime.hour() != 0 || TimerTime.minute() != 0 ||
     TimerTime.second() != 0)
    TimeText += " hh:mm:ss";
  if(!title.isEmpty() && title != "NULL")
    time->setText(title + ": " + datetime.toString(TimeText));
  else
    time->setText(datetime.toString(TimeText));

  timeLeft = new QLabel(this);
  progress = new QProgressBar(this);
  progress->setAlignment(Qt::AlignHCenter);
  if(initMs < datetime.toMSecsSinceEpoch())
    progress->setMaximum(datetime.toMSecsSinceEpoch() - initMs);
#ifdef Q_OS_WIN
  progress->setStyleSheet("QProgressBar::chunk { background-color: #591067;}\
                                QProgressBar {color: #999999;}");
#endif
  QPushButton *remove = new QPushButton("Remove", this);
  QHBoxLayout *layout = new QHBoxLayout(this);

  layout->addWidget(time);
  layout->addWidget(timeLeft);
  layout->addWidget(progress);
  layout->addWidget(remove);

  setLayout(layout);
  connect(remove, SIGNAL(clicked()), this, SLOT(removeThis()));
}

/*!
 * \brief Creates a string that can be saved and loaded
 * \return A QString with the Waiter's title (or NULL) and the msecs since the
 * UNIX epoch it will wait for.
 */
QString WaiterWidget::toLoggableString()
{
  const QString test = (title.isEmpty() ? "NULL" : title.replace(" ", "_")) +
                       ' ' + QString::number(datetime.toMSecsSinceEpoch());
  return ((title.isEmpty() ? "NULL" : title.replace(" ", "_")) + ' ' +
          QString::number(datetime.toMSecsSinceEpoch()) + ' ' +
          repeat.toString());
}

/*!
 * \brief Gets the number of milliseconds since the UNIX epoch.
 * \return The number of milliseconds since the UNIX epoch.
 */
qint64 WaiterWidget::getMsecs() const { return datetime.toMSecsSinceEpoch(); }

/*!
 * \brief Converts from an integer to a string of the month
 * \param month An int from 1 to 12
 * \return A QString with the name of the month, surrounded by spaces.
 */
QString WaiterWidget::intToMonth(int month)
{
  QString returnMe;
  switch(month)
  {
  case 1:
    returnMe = " January ";
    break;
  case 2:
    returnMe = " Feburary ";
    break;
  case 3:
    returnMe = " March ";
    break;
  case 4:
    returnMe = " April ";
    break;
  case 5:
    returnMe = " May ";
    break;
  case 6:
    returnMe = " June ";
    break;
  case 7:
    returnMe = " July ";
    break;
  case 8:
    returnMe = " August ";
    break;
  case 9:
    returnMe = " September ";
    break;
  case 10:
    returnMe = " October ";
    break;
  case 11:
    returnMe = " November ";
    break;
  case 12:
    returnMe = " December ";
  }
  return returnMe;
}

/*!
 * \brief Updates the UI and notifies if an important time is hit.
 * \details Sets the time left in timeLeft, and changes the progress bar.
 * Additionally if the time from t to the end of the timer is important, it
 * notifies the dialog
 * (Important times are 1 year, 1 month, 1 week, 1 day, 12 hours, 6 hours, 1
 * hour, 30 minutes, 15 minutes, 10 minutes, 5 minutes, 1 minute, 0 seconds)
 * \param t The time to set the Waiter to (typically the current time)
 */
void WaiterWidget::update(QDateTime t)
{
  if(quit)
    return;
  if(datetime > t)
  {
    static QTime Zero(0, 0, 0, 0);
    const auto seconds = t.secsTo(datetime);
    if(seconds >= 86400) // 1 day in secs
      timeLeft->setText("<- " + QString::number(t.daysTo(datetime)) + " days");
    else
      timeLeft->setText("<- " + Zero.addSecs(seconds).toString("hh:mm:ss"));
    progress->setValue(t.toMSecsSinceEpoch() - initMs);

    switch(seconds)
    {
    case(31536000) : // 365 * seconds in a day = 1 year
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in one year";
      else
        sayMe =
            "The timer " + title.replace("_", " ") + " will expire in one year";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(2592000) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in one month";
      else
        sayMe = "The timer " + title.replace("_", " ") +
                " will expire in one month";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(604800) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in one week";
      else
        sayMe =
            "The timer " + title.replace("_", " ") + " will expire in one week";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(86400) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in one day";
      else
        sayMe =
            "The timer " + title.replace("_", " ") + " will expire in one day";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(43200) : // 12 * seconds in an hour = 12 hours
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in 12 hours";
      else
        sayMe =
            "The timer " + title.replace("_", " ") + " will expire in 12 hours";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(21600) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in 6 hours";
      else
        sayMe =
            "The timer " + title.replace("_", " ") + " will expire in 6 hours";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(3600) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in an hour";
      else
        sayMe =
            "The timer " + title.replace("_", " ") + " will expire in an hour";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(1800) : // 30 * seconds in one minute = 30 minutes
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in half an hour";
      else
        sayMe = "The timer " + title.replace("_", " ") +
                " will expire in half an hour";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(900) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in 15 minutes";
      else
        sayMe = "The timer " + title.replace("_", " ") +
                " will expire in 15 minutes";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(600) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in 10 minutes";
      else
        sayMe = "The timer " + title.replace("_", " ") +
                " will expire in 10 minutes";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(300) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in 5 minutes";
      else
        sayMe = "The timer " + title.replace("_", " ") +
                " will expire in 5 minutes";
      Q_EMIT speakThis(sayMe);
      break;
    }
    case(60) :
    {
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer will expire in 1 minute";
      else
        sayMe =
            "The timer " + title.replace("_", " ") + " will expire in 1 minute";
      Q_EMIT speakThis(sayMe);
      break;
    }
    }
  }
  else
  {
    timeLeft->setText("");
    progress->setValue(progress->maximum());
    if(progress->isTextVisible())
    {
      progress->setTextVisible(false);
      quit = true;
      QString sayMe;
      if(title == "NULL" || title.isEmpty())
        sayMe = "A timer has expired";
      else
        sayMe = "The timer " + title.replace("_", " ") + " has expired.";
      Q_EMIT speakThis(sayMe);
      if(repeat.repeats())
      {
        try
        {
          const auto &&nextTime = repeat.nextOccurance(datetime);
          Q_EMIT repeatAt(this, title, nextTime, repeat);
        }
        catch(const char *e)
        {
          QString error = QString(e) + " for event " +
                          (title.isNull() ? "" : title) + " at: " +
                          datetime.toString();
          Q_EMIT speakThis(error);
          QMessageBox box;
          box.setText(error);
          box.exec();
        }
      }
    }
  }
}

/*!
 * \brief Tells the WaiterDialog to remove this Waiter. If shift is held down
 * the dialog is instead told to replace the waiter.
 */
void WaiterWidget::removeThis()
{
  if(QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier))
    Q_EMIT replaceAt(this, this->title, this->datetime, repeat);
  else
    Q_EMIT removeAt(this);
}
