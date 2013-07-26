#ifndef SPEAKER_H
#define SPEAKER_H
#include <tbb/concurrent_queue.h>
#include <thread>
//An interface to flite,
//or if someday this is built in Windows, sapi
//Consider libflite?
class Speaker
{
    tbb::concurrent_bounded_queue<std::string> queue;
    bool stopReading;
    bool errorOccurred;
    bool canSendNotifications;
    bool canSpeak;
    void readLoop();
    QString iconLocation;
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

//Exception thrown if speaking fails
class UnableToSpeakException : public std::exception
{
public:
    virtual const char *what()
    {
        return "Unable to invoke flite.";
    }
};

#endif // SPEAKER_H
