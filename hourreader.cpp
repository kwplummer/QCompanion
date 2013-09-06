#include "hourreader.h"
#include <QDateTime>
#include <QString>
#include <QTimer>

/*!
 * \brief Constructs the HourReader and its superclass. Does nothing
 * interesting.
 * \param parent The parent widget owning the component, used for Qt's memory
 * management.
 */
HourReader::HourReader(QWidget *parent) : Component(parent) {
  whenToSpeak.setSingleShot(false);
  whenToSpeak.setInterval(
      QDateTime::currentDateTime().msecsTo(nextCheckTime()));
#if QT_VERSION < 0x050000
  connect(&whenToSpeak, SIGNAL(timeout()), this, SLOT(emitSpeak()));
#else
  connect(&whenToSpeak, &QTimer::timeout, this, &HourReader::emitSpeak);
#endif
  whenToSpeak.start();
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
QDateTime HourReader::nextCheckTime() {
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
QString HourReader::getText() {
  QString debug = "The time is now " + QTime::currentTime().toString("HH") +
                  " hundred hours";
  return ("The time is now " + QTime::currentTime().toString("HH") +
          " hundred hours");
}

/*!
 * \brief Sends the hour to the Speaker, also resets the timer for the next hour
 */
void HourReader::emitSpeak() {
  whenToSpeak.setInterval(
      QDateTime::currentDateTime().msecsTo(nextCheckTime()));
  whenToSpeak.start();
  if (!muted)
    emit wantsToSpeak(getText());
}
