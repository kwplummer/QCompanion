#ifndef SPEAKER_H
#define SPEAKER_H

//An interface to flite,
//or if someday this is built in Windows, sapi
class Speaker
{
public:
    Speaker();
    void speak(const char *speakMe);
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
