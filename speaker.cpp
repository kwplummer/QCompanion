#include <fstream>
#include <QProcess>
#include <libnotify/notify.h>
#include "speaker.h"

Speaker::Speaker(QString iconLocation)
    : stopReading(false),
      errorOccurred(false),
      canSendNotifications(true),
      canSpeak(true),
      iconLocation(iconLocation),
      flite([&](){readLoop();})
{
    notify_init("QCompanion");
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

void Speaker::setNotificationsEnabled(bool enable)
{
    canSendNotifications = enable;
}

void Speaker::setTTSEnabled(bool enable)
{
    canSpeak = enable;
}

bool Speaker::isNotificationsEnabled()
{
    return canSendNotifications;
}

bool Speaker::isTTSEnabled()
{
    return canSpeak;
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
        if(canSendNotifications)
        {
            NotifyNotification *message = notify_notification_new("QCompanion", readMe.c_str(), iconLocation.toUtf8());
            notify_notification_show(message,NULL);
        }
        if(canSpeak)
        {
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
}
