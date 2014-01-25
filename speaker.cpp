#include <QStringList>
#include "speaker.h"
#ifndef Q_OS_WIN
#include "dbusadaptor.h"
#endif

/*!
 * \brief The constructor for Speaker. Starts flite's \link Speaker::readLoop
 * readLoop\endlink
 * \param parent The parent widget, used for Qt's parent/child memory
 * management.
 * \param iconLocation Where the icon used for notifications is located.
 */
Speaker::Speaker(QObject *parent, QString iconLocation)
    : QObject(parent), stopReading(false), canSendNotifications(true),
      canSpeak(true), iconLocation(iconLocation), flite([&]()
                                                        { readLoop(); })
{
#ifndef Q_OS_WIN
  flite_init();
  voice = register_cmu_us_kal(NULL);
  new SpeakerAdaptor(this);
  QDBusConnection dbus = QDBusConnection::sessionBus();
  dbus.registerObject("/Speaker", this);
//  dbus.registerService("com.coderfrog.qcompanion.speaker");
#endif
}

/*!
 * \brief Waits for the thread to finish, and then destroys the Speaker.
 */
Speaker::~Speaker()
{
  if(flite.joinable())
    finishSpeaking();
}

/*!
 * \brief Waits for everything to be read, and stops reading.
 */
void Speaker::finishSpeaking()
{
  stopReading = true;
  queue.push("Stopping");
  flite.join();
}

/*!
 * \brief Sets whether strings should be sent as a notification.
 * \param enable If Notifications should be enabled.
 */
void Speaker::setNotificationsEnabled(bool enable)
{
  canSendNotifications = enable;
}

/*!
 * \brief Sets whether strings should be read aloud
 * \param enable If TTS should be enabled.
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
 * \param speakMe The string to be read aloud, and/or notified.
 */
void Speaker::speak(QString speakMe)
{
  QStringList split = speakMe.split(".");
  for(QString addMe : split)
    queue.push(addMe);
}

/*!
 * \brief The main loop used for speaking and sending notifications.
 * \details The main loop that the speaker runs. While stopReading is false the
 * loop waits for a string to be added to the queue, it pops it out, and then
 * sends it to flite (if canSpeak is enabled) as well as to libnotify (if
 * canSendNotifications are enabled).
 */
void Speaker::readLoop()
{
#ifndef TEST
#ifdef Q_OS_WIN // COM init
  GUID clsid_spvoice = { 0x96749377, 0x3391, 0x11D2, 0x9E, 0xE3, 0x00,
                         0xC0,       0x4F,   0x79,   0x73, 0x96 };
  GUID iid_ispvoice = { 0x6C44DF74, 0x72B9, 0x4992, 0xA1, 0xEC, 0xEF,
                        0x99,       0x6E,   0x04,   0x22, 0xD4 };
  if(FAILED(::CoInitialize(NULL)))
  {
    finishSpeaking();
  }
  HRESULT hr = CoCreateInstance(clsid_spvoice, NULL, CLSCTX_ALL, iid_ispvoice,
                                (void **)&voice);
  if(FAILED(hr))
  {
    finishSpeaking();
  }
#else // D-Bus init
  QDBusInterface notifier("org.freedesktop.Notifications",
                          "/org/freedesktop/Notifications",
                          "org.freedesktop.Notifications");
  QVariantMap hints;
  QList<QVariant> notifierArgs;
  notifierArgs << "QCompanion";  // app_name
  notifierArgs << (uint)0;       // replace_id
  notifierArgs << iconLocation;  // app_icon
  notifierArgs << "QCompanion";  // summary
  notifierArgs << "";            // body (Changes with each call)
  notifierArgs << QStringList(); // actions
  notifierArgs << hints;         // hints
  notifierArgs << (int)0;        // timeout in ms

#endif
// std::this_thread::sleep_for(std::chrono::minutes(1));
#endif // Test's no-sleep
  QString readMe;
  while(!stopReading)
  {
    // Pops from queue, or waits until it can.
    queue.pop(readMe);
#ifndef TEST
#ifndef Q_OS_WIN
    if(canSendNotifications && !readMe.isEmpty())
    {
      notifierArgs[4] = readMe;
      notifier.callWithArgumentList(QDBus::AutoDetect, "Notify", notifierArgs);
    }
    if(canSpeak)
    {
      flite_text_to_speech(readMe.toUtf8(), voice, "play");
    }
    else
    {
      std::this_thread::sleep_for(
          std::chrono::seconds(readMe.split(' ').size()));
    }
#else
    if(canSendNotifications && !readMe.isEmpty())
    {
      Q_EMIT showMessage(readMe);
    }
    if(canSpeak)
    {
      voice->Speak(readMe.toStdWString().c_str(), SPF_DEFAULT, 0);
    }
    else
    {
      std::this_thread::sleep_for(
          std::chrono::seconds(readMe.split(' ').size()));
    }
#endif // Read Message
#endif // Test's skip message
  }
#ifdef Q_OS_WIN
  voice->Release();
  ::CoUninitialize();
#endif // COM uninitialize
}
