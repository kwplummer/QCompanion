#include "hourreader.h"
#include <QDateTime>
#include <QString>
time_t HourReader::nextCheckTime()
{
    auto currentTime_t = time(NULL);
    tm *currentTime = localtime(&currentTime_t);
    ++currentTime->tm_hour;
    currentTime->tm_min=0;
    currentTime->tm_sec=0;
    currentTime->tm_min=0;
    return mktime(currentTime);
}

std::string HourReader::getText()
{
    return ("The time is now " + QTime::currentTime().toString("HH") + " hundred hours").toStdString();
}


extern "C"
{
    Component *makeComponent()
    {
        return new HourReader;
    }
}
