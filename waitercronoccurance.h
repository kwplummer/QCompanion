#ifndef CRONOCCURANCE_H
#define CRONOCCURANCE_H
#include <initializer_list>
#include <QDateTime>
#include <QList>
/*!
 * \brief A struct that holds when an event will repeat.
 * \details Additionally can tell if it is a valid repetition, when it will
 * repeat, and make itself into a string.\n\n In Delay mode the all the times
 * are
 * summed up to create a delay after which the event will occur.\n\n There is
 * alternative mode where instead of having the times be a delay, they are the
 * endpoint. In this other mode if repeatMin is 1 then the event will occur
 * every hour (eg 1:01, 2:01). If repeatHour is 2, and repeatMin is 3 then the
 * event will occur at 2:03 every day.
 */
struct WaiterCronOccurance
{
  WaiterCronOccurance();
  ///\brief Copy constructs a WaiterCronOccurance
  WaiterCronOccurance(const WaiterCronOccurance &) = default;
  WaiterCronOccurance(short repeatMinutes, short repeatHour, short repeatDay,
                      short repeatWeek, short repeatMonth, short repeatYear,
                      bool isDelay, QList<short> repeatDays = QList<short>());
  ///\brief The minute when every repetition will occur.
  short repeatMinutes;
  ///\brief The hour when every repetition will occur.
  short repeatHour;
  /*! \brief The day of the month when repetition will occur.
   * \details If repeatDayOfWeek is also set then the earliest of the two will
   * be chosen.
  */
  short repeatDay;
  ///\brief The week when every repetition will occur.
  short repeatWeek;
  ///\brief The month when every repetition will occur.
  short repeatMonth;
  ///\brief The year when every repetition will occur.
  short repeatYear;
  /*! \brief A list containing the days that repetition will occur on.
   * \details 1-indexed, starting from Monday. (1 = monday, 7 = sunday)
   */
  QList<short> repeatDays;
  /*! \brief Dictates if times are durations or end times.
   *  \details If true "1" in the hour field means every hour, if false it means
   * "every day at 1"
   */
  bool isDelay;

  bool repeats() const;
  QDateTime nextOccurance(QDateTime now) const;
  QString toString() const;
};

#endif // CRONOCCURANCE_H
