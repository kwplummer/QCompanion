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
    queue.push("Stopping");
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
        //Pops from queue, or waits until it can.
        queue.pop(readMe);
        //flite is often buggy for long strings
        //It's much more stable to read from a file.
        std::ofstream out("/tmp/QCompanion");
        out << readMe;
        out.close();
        int exitCode = QProcess::execute("flite -f /tmp/QCompanion");
        if(exitCode < 0)
            errorOccurred = true;
    }
}
