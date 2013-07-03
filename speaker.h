#ifndef SPEAKER_H
#define SPEAKER_H
#include <tbb/concurrent_queue.h>
#include <thread>
//An interface to flite,
//or if someday this is built in Windows, sapi
class Speaker
{
    tbb::concurrent_bounded_queue<std::string> queue;
    bool stopReading;
    bool errorOccurred;
    void readLoop();
    std::thread flite;
public:
    Speaker();
    ~Speaker();
    void speak(const char *speakMe);
    //Returns true if no error occured in speaking.
    //For unit tests.
    bool finishSpeaking();
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
