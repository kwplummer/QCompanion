#include "hourreader.h"
#include <QDateTime>
#include <QString>
QDateTime HourReader::nextCheckTime()
{
    QDateTime returnMe = QDateTime::currentDateTime();
    QTime time = returnMe.time();
    time.setHMS(time.hour(),0,0,0);
    returnMe.setTime(time);
    return returnMe.addSecs(3600);
}

QString HourReader::getText()
{
    return("The time is now " + QTime::currentTime().toString("HH") + " hundred hours");
}
