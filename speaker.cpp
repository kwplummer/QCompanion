#include <QFile>
#include <QProcess>
#include "speaker.h"
Speaker::Speaker()
{
}

void Speaker::speak(const char *speakMe)
{
    //Write what we want to say to disk
    //(We must, as flite is buggy for long strings of text)
    QFile buf("/tmp/QCompanion");
    buf.open(QIODevice::WriteOnly);
    buf.write(QByteArray(speakMe));
    buf.close();
    int exitCode = QProcess::execute("flite -f /tmp/QCompanion");
    //Check if it started without errors. If not, throw
    if(exitCode < 0)
    {
        throw UnableToSpeakException();
    }
}
