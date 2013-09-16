#ifndef CRONOCCURANCE_H
#define CRONOCCURANCE_H
#include <initializer_list>
#include <QDateTime>
/*!
 * \brief A struct that holds when an event will repeat.
 * \details Additionally can tell if it is a valid repetition, when it will
 * repeat, and make itself into a string.
 * If repeatMin is 1 then the event will occur every hour (eg 1:01, 2:01)
 * If repeatHour is 2, and repeatMin is 3 then the event will occur at 2:03
 * every day.
 * \todo Make this a pure virtual base class, and have a "Date Repetition" and
 * "Duration Repetition". Dates would be this style, while Duration would simply
 * be "Every x  hour/minute/day/month/year", where x would be from (-1,0) to
 * INT_MAX. Nothing's stopping us from having something every 999 months! Change
 * the UI to have a radio button that switches between the two sets of options.
 * Depending on this radio button, getResults() would return different derived
 * classes. setResults() wouldn't have an occurrance param, instead it would be
 * overloaded and pick which one is the best based on that.
 */
struct WaiterCronOccurance
{
  WaiterCronOccurance(const WaiterCronOccurance &in);
  WaiterCronOccurance(std::initializer_list<short> list);
  ///\brief The minute when every repetition will occur. -1 for any minute
  short repeatMin;
  ///\brief The hour when every repetition will occur. -1 for any hour
  short repeatHour;
  ///\brief The day of the month when repetition will occur. -1 or 0 for any day
  ///\details If repeatDayOfWeek is also set then the earliest of the two will
  /// be chosen.
  short repeatDayOfMonth;
  ///\brief The month when every repetition will occur. -1 or 0 for any month
  short repeatMonth;
  /*!
   *\brief The day of the week when repetition will occur. -1 or 0 for any day
   *\details If repeatDayOfWeek is also set then the earliest of the two will be
   *chosen. \n\n
   *1 is Monday, 2 is Tuesday, 7 is Sunday.
   */
  short repeatDayOfWeek;
  bool repeats() const;
  QDateTime nextOccurance(const QDateTime &now) const;
  QString toString() const;
};

#endif // CRONOCCURANCE_H
