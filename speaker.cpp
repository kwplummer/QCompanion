#include <libnotify/notify.h>
#include <QStringList>
#include "speaker.h"

/*!
 * \brief The constructor for Speaker. Starts flite's \link Speaker::readLoop
 * readLoop\endlink
 * \param[in] iconLocation Where the icon used for notifications is located.
 */
Speaker::Speaker(QString iconLocation)
    : stopReading(false), canSendNotifications(true), canSpeak(true),
      iconLocation(iconLocation), flite([&]() { readLoop(); }) {
  notify_init("QCompanion");
  flite_init();
  voice = register_cmu_us_kal(NULL);
}

/*!
 * \brief Waits for the thread to finish, and then destroys the Speaker.
 */
Speaker::~Speaker() {
  if (flite.joinable())
    finishSpeaking();
}

/*!
 * \brief Waits for everything to be read, and stops reading.
 * \return true if no error occured when speaking.
 */
void Speaker::finishSpeaking() {
  stopReading = true;
  queue.push("Stopping");
  flite.join();
}

/*!
 * \brief Sets whether strings should be sent as a notification.
 * \param[in] enable If Notifications should be enabled.
 */
void Speaker::setNotificationsEnabled(bool enable) {
  canSendNotifications = enable;
}

/*!
 * \brief Sets whether strings should be read aloud
 * \param[in] enable If TTS should be enabled.
 */
void Speaker::setTTSEnabled(bool enable) { canSpeak = enable; }

/*!
 * \brief Returns if notifications are enabled. If they are, then the program
 * will try to send notifications using libnotify
 * \return A boolean indicating if future \link Speaker::speak speak() \endlink
 * calls will send a notification.
 */
bool Speaker::isNotificationsEnabled() { return canSendNotifications; }

/*!
 * \brief Returns if "TTS" is enabled. If it is, then the program will invoke
 * flite to speak aloud.
 * \return A boolean indicating if future \link Speaker::speak speak() \endlink
 * calls will be read aloud.
 */
bool Speaker::isTTSEnabled() { return canSpeak; }

/*!
 * \brief Enqueues a const char * to be spoken on the next run of
 * Speaker::readLoop.
 * If there are any errors in the last loop then it throws an exception.
 * libflite "tires" when speaking for a long amount of time, so multi-sentence
 * notifications are split into individual sentences.
 * \param[in] speakMe The string to be read aloud, and/or notified.
 */
void Speaker::speak(QString speakMe) {
  QStringList split = speakMe.split(".");
  for (QString addMe : split)
    queue.push(addMe);
}

/*!
 * \brief The main loop used for speaking and sending notifications.
 * \details The main loop that the speaker runs. While stopReading is false the
 * loop waits for a string to be added to the queue, it pops it out, and then
 * sends it to flite (if canSpeak is enabled) as well as to libnotify (if
 * canSendNotifications are enabled).
 */
void Speaker::readLoop() {
  QString readMe;
  while (!stopReading) {
    // Pops from queue, or waits until it can.
    queue.pop(readMe);
#ifndef TEST
    NotifyNotification *message = nullptr;
    if (canSendNotifications && !readMe.isEmpty()) {
      message = notify_notification_new("QCompanion", readMe.toUtf8(),
                                        iconLocation.toUtf8());
      notify_notification_show(message, NULL);
    }
    if (canSpeak) {
      flite_text_to_speech(readMe.toUtf8(), voice, "play");
      if (message)
        notify_notification_close(message, nullptr);
      message = nullptr;
    } else {
      std::this_thread::sleep_for(
          std::chrono::seconds(readMe.split(' ').size()));
    }
    message = nullptr;
#endif
  }
}
