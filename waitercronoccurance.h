#ifndef CRONOCCURANCE_H
#define CRONOCCURANCE_H
#include <initializer_list>
struct WaiterCronOccurance {
  WaiterCronOccurance(const WaiterCronOccurance &in);
  WaiterCronOccurance(std::initializer_list<short> list);
  short repeatMin;
  short repeatHour;
  short repeatDayOfMonth;
  short repeatMonth;
  short repeatDayOfWeek;
};

#endif // CRONOCCURANCE_H
