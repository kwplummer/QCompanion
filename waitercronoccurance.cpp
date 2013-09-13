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
 * an std::initializer_list of shorts. If there are less than 5 items the items
 * are initialized to the same values a default WaiterCronDialog would contain.
 * Any items after the first five are ignored.
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
  QDateTime next(now);
  if(repeatMin != -1)
  {
    const auto nowMin = next.time().minute();
    if(repeatMin > nowMin)
    {
      next = next.addSecs((repeatMin - nowMin) * 60);
    }
    else
    {
      next = next.addSecs((60 - nowMin + repeatMin) * 60);
    }
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
    else
    {
      next = next.addSecs((24 - nowHour + repeatHour) * 60 * 60);
    }
  }
  else if(repeatMin == -1)
  {
    auto nextTime = next.time();
    nextTime.setHMS(0, nextTime.minute(), 0);
    next.setTime(nextTime);
  }
  if(repeatDayOfMonth > 0 && repeatDayOfWeek != 0)
  {
    int DoMSecs = 0;
    int DoWSecs = 0;
    const auto nowDoM = next.date().day();
    if(repeatDayOfMonth > nowDoM)
    {
      DoMSecs = (repeatDayOfMonth - nowDoM) * 24 * 60 * 60;
    }
    else
    {
      DoMSecs = (30 - nowDoM + repeatDayOfMonth) * 24 * 60 * 60;
    }
    // TODO: Check this one! Does QDate::dayOfWeek() return a 0-indexed or
    // 1-indexed value?
    const auto nowWeekday = next.date().dayOfWeek();
    if(repeatDayOfWeek > nowWeekday)
    {
      DoWSecs = (repeatDayOfWeek - nowWeekday) * 24 * 60 * 60;
    }
    else
    {
      DoWSecs = (7 - nowWeekday + repeatDayOfWeek) * 24 * 60 * 60;
    }
    next = next.addSecs(DoMSecs < DoWSecs ? DoWSecs : DoMSecs);
  }
  else
  {
    if(repeatDayOfMonth > 0)
    {
      const auto nowDoM = next.date().day();
      if(repeatDayOfMonth > nowDoM)
      {
        next = next.addSecs((repeatDayOfMonth - nowDoM) * 24 * 60 * 60);
      }
      else
      {
        next = next.addSecs((30 - nowDoM + repeatDayOfMonth) * 24 * 60 * 60);
      }
    }
    else if(repeatDayOfWeek != 0)
    {
      const auto nowWeekday = next.date().dayOfWeek();
      if(repeatDayOfWeek > nowWeekday)
      {
        next = next.addSecs((repeatDayOfWeek - nowWeekday) * 24 * 60 * 60);
      }
      else
      {
        next = next.addSecs((7 - nowWeekday + repeatDayOfWeek) * 24 * 60 * 60);
      }
    }
  }
  if(repeatMonth > 0)
  {
    const auto nowMonth = next.date().month();
    if(repeatMonth > nowMonth)
    {
      next = next.addSecs((repeatMonth - nowMonth) * 30 * 24 * 60 * 60);
    }
    else
    {
      next = next.addSecs((12 - nowMonth + repeatMonth) * 30 * 24 * 60 * 60);
    }
  }
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
