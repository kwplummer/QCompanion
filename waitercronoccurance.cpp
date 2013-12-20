#include "waitercronoccurance.h"
#include <map>

/*!
 * \brief WaiterCronOccurance::WaiterCronOccurance Constructs the
 * WaiterCronOccurance from a list.
 * \param repeatMinutes The number of minutes used for repetition.
 * \param repeatHour The number of hour used for repetition.
 * \param repeatDay The number of day used for repetition.
 * \param repeatWeek The number of week used for repetition.
 * \param repeatMonth The number of month used for repetition.
 * \param repeatYear The number of year used for repetition.
 * \param isDelay If the repetition is a delay, or the end of the event.
 * \param repeatDays The days on which repetition will occur. Not used in delay
 * mode.
 */
WaiterCronOccurance::WaiterCronOccurance(short repeatMinutes, short repeatHour,
                                         short repeatDay, short repeatWeek,
                                         short repeatMonth, short repeatYear,
                                         bool isDelay, QList<short> repeatDays)
    : repeatMinutes(repeatMinutes), repeatHour(repeatHour),
      repeatDay(repeatDay), repeatWeek(repeatWeek), repeatMonth(repeatMonth),
      repeatYear(repeatYear), repeatDays(repeatDays), isDelay(isDelay)
{
  if(!isDelay)
  {
    this->repeatWeek = this->repeatYear = 0;
  }
  else
  {
    repeatDays.clear();
  }
}

WaiterCronOccurance::WaiterCronOccurance()
    : WaiterCronOccurance(0, 0, 0, 0, 0, 0, true) {}

/*!
 * \brief Determines if the occurance has any values that would cause it to
 * repeat.
 * \return If the occurance will repeat.
 */
bool WaiterCronOccurance::repeats() const
{
  return repeatMinutes != 0 || repeatHour != 0 || repeatDay > 0 ||
         repeatWeek > 0 || repeatMonth > 0 || repeatYear > 0 ||
         (!isDelay && !repeatDays.empty());
}
/*!
 * \brief Returns if a date is valid according to the cron-style repetition
 * rules
 * \param when The time to test
 * \return If the date is a valid repetition.
 */
bool WaiterCronOccurance::isValidRepetition(QDateTime when) const
{
  /*Day/Weekday logic, in if/else, rather than boolean algebra:
  if(repeatDay)
  {
    if(when.date().day() == repeatDay)
    {
      true;
    }
    if(repeatDays.contains(when.date().dayOfWeek()))
    {
      true;
    }
  }
  else if(!repeatDays.empty())
  {
    if(repeatDays.contains(when.date().dayOfWeek()))
    {
      true;
    }
  }
  else
  {
    true;
  }
  */
  return (repeatMinutes ? when.time().minute() == repeatMinutes : true) &&
         (repeatHour ? when.time().hour() == repeatHour : true) &&
         (repeatMonth ? when.date().month() == repeatMonth : true) &&
         ((repeatDay && (when.date().day() == repeatDay ||
                         repeatDays.contains(when.date().dayOfWeek()))) ||
          (!repeatDay && ((!repeatDays.empty() &&
                           repeatDays.contains(when.date().dayOfWeek())) ||
                          repeatDays.empty())));
}
/*!
 * \brief Returns when the next repetition should occur.
 * \param now The time from which the "next" time is calculated.
 * \todo Further test what happens when multiple times are out there at once.
 * Last time having every single day selected and 30 minutes selected
 * resulted in the event not happening at 00:30, but 20:30, as that was the time
 * that was already there. We want to select the first _possible_ time for
 * an event to occur, not _a_ time an event can occur!
 * \return When the next repetition occurs.
 */
QDateTime WaiterCronOccurance::nextOccurance(QDateTime now) const
{
  if(!repeats())
    return QDateTime::fromTime_t(0);
  if(!isDelay)
  {
    QTime nowTime = now.time();
    nowTime.setHMS(nowTime.hour(), nowTime.minute(), 0);
    QDateTime next(now.date(), nowTime);
    // Set the next occurance to be at least one minute from now, to prevent
    // infinite loops.
    next = next.addSecs(60);
    // To prevent an "Every friday" repetition from always evaluating to true if
    // the source event is on a friday, only check if a repetition is valid if
    // the minute is a factor.
    const bool minSet = repeatMinutes != 0;
    while(next.time().minute() != repeatMinutes)
    {
      next = next.addSecs(60);
      if(minSet && isValidRepetition(next))
        return next;
    }
    // To prevent an "Every friday" repetition from always evaluating to true if
    // the source event is on a friday, only check if a repetition is hour if
    // the minute is a factor.
    const bool hourSet = repeatHour != 0;
    while(next.time().hour() != repeatHour)
    {
      next = next.addSecs(60 * 60);
      if(hourSet && isValidRepetition(next))
        return next;
    }
    if(repeatMonth != 0)
    {
      while(next.date().month() != repeatMonth)
      {
        next = next.addDays(1);
        if(isValidRepetition(next))
          return next;
      }
    }
    if(repeatDays.isEmpty())
    {
      if(repeatDay == 0)
      {
        for(int i = 0; i < 24 * 60 * 60; ++i)
        {
          next = next.addSecs(1);
          if(isValidRepetition(next))
            return next;
        }
      }
      else
      {
        while(next.date().day() != repeatDay)
        {
          next = next.addDays(1);
          if(isValidRepetition(next))
            return next;
        }
      }
    }
    else
    {
      if(isValidRepetition(next))
        return next;
      for(int i = 0; i < 7; ++i)
      {
        next = next.addDays(1);
        if(isValidRepetition(next))
          return next;
      }
      if(repeatDay != 0)
      {
        while(next.date().day() != repeatDay)
        {
          next = next.addDays(1);
          if(isValidRepetition(next))
            return next;
        }
      }
    }
    throw "Unable to find valid repetition";
  }
  if(repeatMinutes > 0)
    now = now.addSecs(repeatMinutes * 60);
  if(repeatHour > 0)
    now = now.addSecs(repeatHour * 60 * 60);
  if(repeatDay > 0)
    now = now.addDays(repeatDay);
  if(repeatWeek > 0)
    now = now.addDays(repeatWeek * 7);
  if(repeatMonth > 0)
    now = now.addMonths(repeatMonth);
  if(repeatYear > 0)
    now = now.addYears(repeatYear);
  return now;
}
/*!
 * \brief Makes a string writable to a state file.
 * \return A string containing every value in the struct, interspersed with
 * spaces.
 */
QString WaiterCronOccurance::toString() const
{
  QString returnMe;
  for(short s : repeatDays)
  {
    returnMe.append(' ' + QString::number(s));
  }
  return QString::number(repeatMinutes) + ' ' + QString::number(repeatHour) +
         ' ' + QString::number(repeatDay) + ' ' + QString::number(repeatWeek) +
         ' ' + QString::number(repeatMonth) + ' ' +
         QString::number(repeatYear) + ' ' + QString::number(isDelay) +
         returnMe;
}
