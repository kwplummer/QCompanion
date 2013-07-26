#include "hourreader.h"
#include <QDateTime>
#include <QString>

/*!
 * \brief Constructs the HourReader and its superclass. Does nothing interesting.
 * \param parent The parent widget owning the component, used for Qt's memory management.
 */
HourReader::HourReader(QWidget *parent): Component(parent)
{
}

/*!
 * \brief Destroys the HourReader, does nothing interesting.
 */
HourReader::~HourReader()
{
}

/*!
 * \brief Gets when the component's text should be shown
 * \details Sets the minutes and seconds to 0, and adds one hour.
 * \return A QDateTime when the component should be read.
 */
QDateTime HourReader::nextCheckTime()
{
    QDateTime returnMe = QDateTime::currentDateTime();
    QTime time = returnMe.time();
    time.setHMS(time.hour(),0,0,0);
    returnMe.setTime(time);
    return returnMe.addSecs(3600);
}

/*!
 * \brief The current hour
 * \return A QString with the hour read as military time.
 */
QString HourReader::getText()
{
    return("The time is now " + QTime::currentTime().toString("HH") + " hundred hours");
}


