#ifndef SPEAKER_H
#define SPEAKER_H
#include <tbb/concurrent_queue.h>
#include <thread>
/*!
 * \brief Offers a queue and an interface to flite and libnotify.
 * \todo Incorporate libflite instead of calling the program.
 */
class Speaker
{
    ///\brief The concurrent queue that is used to store strings to be read/notified.
    tbb::concurrent_bounded_queue<std::string> queue;
    ///\brief Set to true in the destructor, used to specify that the loop should not continue.
    bool stopReading;
    ///\brief Set to true of flite exits abnormally
    bool errorOccurred;
    ///\brief checked to indicate whether strings should be sent as a notification
    bool canSendNotifications;
    ///\brief checked to indicate whether strings should be spoken aloud.
    bool canSpeak;
    void readLoop();
    ///\brief Where the icon used for notifications is located.
    QString iconLocation;
    ///\brief The thread that will run \link Speaker::readLoop readLoop \endlink and check the queue, speaking and notifying when strings come in.
    std::thread flite;
public:
    Speaker(QString iconLocation);
    ~Speaker();
    void speak(const char *speakMe);
    //Returns true if no error occured in speaking.
    //For unit tests.
    bool finishSpeaking();
    void setNotificationsEnabled(bool enable);
    void setTTSEnabled(bool enable);
    bool isNotificationsEnabled();
    bool isTTSEnabled();
};

/*!
 * \brief An exception thrown when there are speech errors.
 * \details Thrown when there's an error executing the flite program.
 */
class UnableToSpeakException : public std::exception
{
public:
    /*!
     * \return A string stating that flite could not be invoked
     */
    virtual const char *what()
    {
        return "Unable to invoke flite.";
    }
};

#endif // SPEAKER_H
