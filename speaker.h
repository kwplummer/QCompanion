#ifndef SPEAKER_H
#define SPEAKER_H
#include <tbb/concurrent_queue.h>
#include <thread>
#include <QString>
#include <QObject>
#ifndef Q_OS_WIN
#include <flite/flite.h>
extern "C" cst_voice *register_cmu_us_kal(const char *voxdir);
typedef cst_voice Voice;
#else
#include <sapi.h>
typedef ISpVoice Voice;
#endif
///\brief Offers a queue and an interface to text to speech and notifications.
class Speaker : public QObject
{
  Q_OBJECT
  /*!
   * \brief The concurrent queue that is used to store strings to be
   * read/notified.
   */
  tbb::concurrent_bounded_queue<QString> queue;
  /*!
   * \brief Set to true in the destructor, used to specify that the loop should
   * not continue.
   */
  bool stopReading;
  /*! \brief checked to indicate whether strings should be sent as a
   * notification
   */
  bool canSendNotifications;
  ///\brief checked to indicate whether strings should be spoken aloud.
  bool canSpeak;
  void readLoop();
  ///\brief Where the icon used for notifications is located.
  QString iconLocation;
  /*!
   * \brief The thread that will run \link Speaker::readLoop readLoop \endlink
   * and check the queue, speaking and notifying when strings come in.
   */
  std::thread flite;
  ///\brief A handle to the voice used for text to speech.
  Voice *voice;

public:
  Speaker(QObject *parent, QString iconLocation);
  virtual ~Speaker();
  void speak(QString speakMe);
  // Returns true if no error occured in speaking.
  // For unit tests.
  void finishSpeaking();
  void setNotificationsEnabled(bool enable);
  void setTTSEnabled(bool enable);
  bool isNotificationsEnabled();
  bool isTTSEnabled();
  /*!
   * \brief Tells the UI thread to show a message
   * \param message What to display
   */
  Q_SIGNAL void showMessage(QString message);
};
#endif // SPEAKER_H
