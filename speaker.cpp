#include <fstream>
#include <QProcess>
#include "speaker.h"

Speaker::Speaker()
    : stopReading(false),
      errorOccurred(false),
      flite([&](){readLoop();})
{
}

Speaker::~Speaker()
{
    if(flite.joinable())
        finishSpeaking();
}

//Waits for everything to be read, and stops reading.
//Returns true if no error occured when speaking.
bool Speaker::finishSpeaking()
{
    stopReading = true;
    flite.join();
    return !errorOccurred;
}

//Adds a const char *to a queue to be read.
//One thing is read per minute.
void Speaker::speak(const char *speakMe)
{
    queue.push(speakMe);
    if(errorOccurred)
        throw UnableToSpeakException();
}

//The main loop that reads text.
void Speaker::readLoop()
{
    std::string readMe;
    while(!stopReading)
    {
        //Tries to pop from the queue into readme.
        //It speaks if there is something to pop
        if(queue.try_pop(readMe))
        {
            //flite is often buggy for long strings
            //It's much more stable to read from a file.
            std::ofstream out("/tmp/QCompanion");
            out << readMe;
            out.close();
            int exitCode = QProcess::execute("flite -f /tmp/QCompanion");
            if(exitCode < 0)
            {
                errorOccurred = true;
                return;
            }
        }
        //To avoid aggrivating the user, we only talk once a minute
        //This is checked twice as the condition could change during sleep.
        //However we don't need to wait one minute per unit test.
#ifndef TEST
        if(!stopReading)
            sleep(60);
#endif
    }
}
