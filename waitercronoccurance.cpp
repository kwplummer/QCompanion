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
    : WaiterCronOccurance(0, 0, 0, 0, 0, 0, true)
{
}

/*!
 * \brief Determines if the occurance has any values that would cause it to
 * repeat.
 * \return If the occurance will repeat.
 */
bool WaiterCronOccurance::repeats() const
{
  return repeatMinutes != 0 || repeatHour != 0 || repeatDay > 0 ||
         repeatWeek > 0 || repeatMonth > 0 || repeatYear > 0 ||
         !repeatDays.empty();
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

    if(repeatMinutes != 0)
    {
      const auto nowMin = next.time().minute();
      if(repeatMinutes > nowMin)
      {
        next = next.addSecs((repeatMinutes - nowMin) * 60);
      }
      else if(repeatMinutes < nowMin)
      {
        next = next.addSecs((60 - nowMin + repeatMinutes) * 60);
      }
      else if(repeatHour == 0 && repeatDay == 0 && repeatMonth <= 0 &&
              repeatDays.empty())
        next = next.addSecs(60 * 60);
    }
    else
    {
      auto nextTime = next.time();
      nextTime.setHMS(nextTime.hour(), 0, 0);
      next.setTime(nextTime);
    }
    if(repeatHour != 0)
    {
      const auto nowHour = next.time().hour();
      if(repeatHour > nowHour)
      {
        next = next.addSecs((repeatHour - nowHour) * 60 * 60);
      }
      else if(repeatHour < nowHour)
      {
        next = next.addSecs((24 - nowHour + repeatHour) * 60 * 60);
      }
      else if(repeatMinutes == 0 && repeatDay == 0 && repeatMonth <= 0 &&
              repeatDays.empty())
        next = next.addDays(1);
    }
    else if(repeatMinutes == 0 || repeatMonth > 0 || repeatDay > 0 ||
            !repeatDays.empty())
    {
      auto nextTime = next.time();
      nextTime.setHMS(0, nextTime.minute(), 0);
      next.setTime(nextTime);
    }

    if(repeatMonth > 0)
    {
      const auto nowMonth = next.date().month();
      if(repeatMonth > nowMonth)
      {
        next = next.addMonths(repeatMonth - nowMonth);
      }
      else if(repeatMonth < nowMonth)
      {
        next = next.addMonths(12 - nowMonth + repeatMonth);
      }
      else if(repeatDay <= 0 && repeatDays.empty() && repeatHour == 0 &&
              repeatMinutes == 0)
        next = next.addMonths(12);
      auto nextDate = next.date();
      nextDate.setYMD(nextDate.year(), nextDate.month(), 1);
      next.setDate(nextDate);
    }

    if(repeatDay > 0 && !repeatDays.empty())
    {
      int DoM = 0;
      int DoW = 0;
      const auto nowDoM = next.date().day();
      if(repeatDay > nowDoM)
      {
        DoM = (repeatDay - nowDoM);
      }
      else if(repeatDay < nowDoM)
      {
        DoM = (next.date().daysInMonth() - nowDoM + repeatDay);
      }
      else if(repeatMonth <= 0 && repeatHour == 0 && repeatMinutes == 0)
        DoM = next.date().daysInMonth();
      const auto nowWeekday = next.date().dayOfWeek();
      std::pair<short, short> nearestDay = std::make_pair(9000, 9000);
      for(short s : repeatDays)
      {
        if(s < nowWeekday)
        {
          if(7 - nowWeekday + s < nearestDay.second)
          {
            nearestDay.second = 7 - nowWeekday + s;
            nearestDay.first = s;
          }
        }
        else if(nowWeekday - s < nearestDay.second)
        {
          nearestDay.second = nowWeekday - s;
          nearestDay.first = s;
        }
      }
      if(nearestDay.first > nowWeekday)
      {
        DoW = (nearestDay.first - nowWeekday);
      }
      else if(nearestDay.first < nowWeekday)
      {
        DoW = (7 - nowWeekday + nearestDay.first);
      }
      else if(repeatMonth <= 0 && repeatHour == 0 && repeatMinutes == 0)
        DoW = 7;
      next = next.addDays(DoM < DoW ? DoM : DoW);
    }
    else
    {
      if(repeatDay > 0)
      {
        const auto nowDoM = next.date().day();
        if(repeatDay > nowDoM)
        {
          next = next.addDays(repeatDay - nowDoM);
        }
        else if(repeatDay < nowDoM)
        {
          next = next.addDays(next.date().daysInMonth() - nowDoM + repeatDay);
        }
        else if(repeatMonth <= 0 && repeatHour == 0 && repeatMinutes == 0)
          next = next.addMonths(1);
      }
      else if(!repeatDays.empty())
      {
        const auto nowWeekday = next.date().dayOfWeek();
        std::pair<short, short> nearestDay = std::make_pair(9000, 9000);
        for(short s : repeatDays)
        {
          if(s < nowWeekday)
          {
            if(7 - nowWeekday + s < nearestDay.second)
            {
              nearestDay.second = 7 - nowWeekday + s;
              nearestDay.first = s;
            }
          }
          else if(nowWeekday - s < nearestDay.second)
          {
            nearestDay.second = nowWeekday - s;
            nearestDay.first = s;
          }
        }
        if(nearestDay.first > nowWeekday)
        {
          next = next.addDays(nearestDay.first - nowWeekday);
        }
        else if(nearestDay.first < nowWeekday)
        {
          next = next.addDays(7 - nowWeekday + nearestDay.first);
        }
        else if(repeatMonth <= 0 && repeatHour == 0 && repeatMinutes == 0)
          next = next.addDays(7);
      }
    }
    // QString DEBUGDELETETHIS2 = next.toString();
    return next;
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
