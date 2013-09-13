#ifndef SPEAKER_H
#define SPEAKER_H
#include <tbb/concurrent_queue.h>
#include <thread>
#include <QString>
#include <flite/flite.h>

extern "C" cst_voice *register_cmu_us_kal(const char *voxdir);
///\brief Offers a queue and an interface to flite and libnotify.
class Speaker
{
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
  cst_voice *voice;

public:
  Speaker(QString iconLocation);
  ~Speaker();
  void speak(QString speakMe);
  // Returns true if no error occured in speaking.
  // For unit tests.
  void finishSpeaking();
  void setNotificationsEnabled(bool enable);
  void setTTSEnabled(bool enable);
  bool isNotificationsEnabled();
  bool isTTSEnabled();
};

#endif // SPEAKER_H
