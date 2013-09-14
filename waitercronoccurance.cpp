#include "waitercronoccurance.h"
/*!
 * \brief Copies the occurance from another WaiterCronOccurance
 * \param in the occurance to copy from.
 */
WaiterCronOccurance::WaiterCronOccurance(const WaiterCronOccurance &in)
    : repeatMin(in.repeatMin), repeatHour(in.repeatHour),
      repeatDayOfMonth(in.repeatDayOfMonth), repeatMonth(in.repeatMonth),
      repeatDayOfWeek(in.repeatDayOfWeek)
{
}
/*!
 * \brief Constructs the WaiterCronOccurance from a list
 * \param list the list used to fill the occurance
 * \details Constructs the WaiterCronOccurance from the first five elements in
 * an std::initializer_list of shorts. If there are less than 5 items then
 * everything is initialized to the same values a default WaiterCronDialog would
 * contain. Any items after the first five are ignored.
 */
WaiterCronOccurance::WaiterCronOccurance(std::initializer_list<short> list)
{
  if(list.size() >= 5)
  {
    auto itr = list.begin();
    repeatMin = *itr++;
    repeatHour = *itr++;
    repeatDayOfMonth = *itr++;
    repeatMonth = *itr++;
    repeatDayOfWeek = *itr;
  }
  else
  {
    repeatMin = repeatHour = repeatDayOfMonth = repeatMonth = -1;
    repeatDayOfWeek = 0;
  }
}
/*!
 * \brief Determines if the occurance has any values that would cause it to
 * repeat.
 * \return If the occurance will repeat.
 */
bool WaiterCronOccurance::repeats() const
{
  return repeatMin != -1 || repeatHour != -1 || repeatDayOfMonth > 0 ||
         repeatMonth > 0 || repeatDayOfWeek != 0;
}
/*!
 * \brief Returns when the next repetition should occur.
 * \param now The time from which the "next" time is calculated.
 * \return When the next repetition occurs.
 */
QDateTime WaiterCronOccurance::nextOccurance(const QDateTime &now) const
{
  if(!repeats())
    return QDateTime::fromTime_t(0);
  QTime nowTime = now.time();
  nowTime.setHMS(nowTime.hour(), nowTime.minute(), 0);
  QDateTime next(now.date(), nowTime);

  if(repeatMin != -1)
  {
    const auto nowMin = next.time().minute();
    if(repeatMin > nowMin)
    {
      next = next.addSecs((repeatMin - nowMin) * 60);
    }
    else if(repeatMin < nowMin)
    {
      next = next.addSecs((60 - nowMin + repeatMin) * 60);
    }
    else if(repeatHour == -1 && repeatDayOfMonth == -1 && repeatMonth <= 0 &&
            repeatDayOfWeek == 0)
      next.addSecs(60 * 60);
  }
  else
  {
    auto nextTime = next.time();
    nextTime.setHMS(nextTime.hour(), 0, 0);
    next.setTime(nextTime);
  }
  if(repeatHour != -1)
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
    else if(repeatMin == -1 && repeatDayOfMonth == -1 && repeatMonth <= 0 &&
            repeatDayOfWeek == 0)
      next = next.addDays(1);
  }
  else if(repeatMin == -1)
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
    else if(repeatDayOfMonth <= 0 && repeatDayOfWeek == -1 &&
            repeatHour == -1 && repeatMin == -1)
      next = next.addMonths(12);
    auto nextDate = next.date();
    nextDate.setYMD(nextDate.year(), nextDate.month(), 1);
    next.setDate(nextDate);
  }
  QString DEBUGDELETETHIS = next.toString();

  if(repeatDayOfMonth > 0 && repeatDayOfWeek != 0)
  {
    int DoM = 0;
    int DoW = 0;
    const auto nowDoM = next.date().day();
    if(repeatDayOfMonth > nowDoM)
    {
      DoM = (repeatDayOfMonth - nowDoM);
    }
    else if(repeatDayOfMonth < nowDoM)
    {
      DoM = (next.date().daysInMonth() - nowDoM + repeatDayOfMonth);
    }
    else if(repeatMonth <= 0 && repeatHour == -1 && repeatMin == -1)
      DoM = next.date().daysInMonth();
    const auto nowWeekday = next.date().dayOfWeek();
    if(repeatDayOfWeek > nowWeekday)
    {
      DoW = (repeatDayOfWeek - nowWeekday);
    }
    else if(repeatDayOfWeek < nowWeekday)
    {
      DoW = (7 - nowWeekday + repeatDayOfWeek);
    }
    else if(repeatMonth <= 0 && repeatHour == -1 && repeatMin == -1)
      DoW = 7;
    next = next.addDays(DoM < DoW ? DoM : DoW);
  }
  else
  {
    if(repeatDayOfMonth > 0)
    {
      const auto nowDoM = next.date().day();
      if(repeatDayOfMonth > nowDoM)
      {
        next = next.addDays(repeatDayOfMonth - nowDoM);
      }
      else if(repeatDayOfMonth < nowDoM)
      {
        next =
            next.addDays(next.date().daysInMonth() - nowDoM + repeatDayOfMonth);
      }
      else if(repeatMonth <= 0 && repeatHour == -1 && repeatMin == -1)
        next = next.addMonths(1);
    }
    else if(repeatDayOfWeek != 0)
    {
      const auto nowWeekday = next.date().dayOfWeek();
      if(repeatDayOfWeek > nowWeekday)
      {
        next = next.addDays(repeatDayOfWeek - nowWeekday);
      }
      else if(repeatDayOfWeek < nowWeekday)
      {
        next = next.addDays(7 - nowWeekday + repeatDayOfWeek);
      }
      else if(repeatMonth <= 0 && repeatHour == -1 && repeatMin == -1)
        next = next.addDays(7);
    }
  }

  QString DEBUGDELETETHIS2 = next.toString();
  return next;
}
/*!
 * \brief Makes a string writable to a state file.
 * \return A string containing every value in the struct, interspersed with
 * spaces.
 */
QString WaiterCronOccurance::toString() const
{
  return QString::number(repeatMin) + ' ' + QString::number(repeatHour) + ' ' +
         QString::number(repeatDayOfMonth) + ' ' +
         QString::number(repeatMonth) + ' ' + QString::number(repeatDayOfWeek);
}
