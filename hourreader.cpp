#include "hourreader.h"
#include <QDateTime>
#include <QString>
#include <QTimer>
#ifndef Q_OS_WIN
#include "dbusadaptor.h"
#endif

/*!
 * \brief Constructs the HourReader and its superclass. Does nothing
 * interesting.
 * \param parent The parent widget owning the component, used for Qt's memory
 * management.
 */
HourReader::HourReader(QWidget *parent) : Component(parent)
{
  whenToSpeak.setSingleShot(false);
  whenToSpeak.setInterval(
      QDateTime::currentDateTime().msecsTo(nextCheckTime()));
  connect(&whenToSpeak, SIGNAL(timeout()), this, SLOT(emitSpeak()));
  whenToSpeak.start();
#ifndef Q_OS_WIN
  new HourreaderAdaptor(this);
  QDBusConnection dbus = QDBusConnection::sessionBus();
  dbus.registerObject("/HourReader", this);
// dbus.registerService("com.coderfrog.qcompanion.hourreader");
#endif
}

/*!
 * \brief Destroys the HourReader, does nothing interesting.
 */
HourReader::~HourReader() {}

/*!
 * \brief Gets when the component's text should be shown
 * \details Sets the minutes and seconds to 0, and adds one hour.
 * \return A QDateTime when the component should be read.
 */
QDateTime HourReader::nextCheckTime()
{
  QDateTime returnMe = QDateTime::currentDateTime();
  QTime time = returnMe.time();
  time.setHMS(time.hour(), 0, 0, 0);
  returnMe.setTime(time);
  return returnMe.addSecs(3600);
}

/*!
 * \brief The current hour
 * \return A QString with the hour read as military time.
 */
QString HourReader::getText()
{
  const auto time = QTime::currentTime();
  if(time.hour() == 0)
    return ("The time is now zero hundred hours");
  return ("The time is now " + time.toString("HH") + " hundred hours");
}

/*!
 * \brief Sends the hour to the Speaker, also resets the timer for the next hour
 */
void HourReader::emitSpeak()
{
  whenToSpeak.setInterval(
      QDateTime::currentDateTime().msecsTo(nextCheckTime()));
  whenToSpeak.start();
  if(!muted)
    Q_EMIT wantsToSpeak(getText());
}
