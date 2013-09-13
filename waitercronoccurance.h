#ifndef CRONOCCURANCE_H
#define CRONOCCURANCE_H
#include <initializer_list>
#include <QDateTime>
struct WaiterCronOccurance
{
  WaiterCronOccurance(const WaiterCronOccurance &in);
  WaiterCronOccurance(std::initializer_list<short> list);
  short repeatMin;
  short repeatHour;
  short repeatDayOfMonth;
  short repeatMonth;
  short repeatDayOfWeek;
  bool repeats() const;
  QDateTime nextOccurance(const QDateTime &now) const;
  QString toString() const;
};

#endif // CRONOCCURANCE_H
