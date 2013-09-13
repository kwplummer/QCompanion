#include "waitercronoccurance.h"

WaiterCronOccurance::WaiterCronOccurance(const WaiterCronOccurance &in)
    : repeatMin(in.repeatMin), repeatHour(in.repeatHour),
      repeatDayOfMonth(in.repeatDayOfMonth), repeatMonth(in.repeatMonth),
      repeatDayOfWeek(in.repeatDayOfWeek) {}

WaiterCronOccurance::WaiterCronOccurance(std::initializer_list<short> list) {
  if (list.size() == 5) {
    auto itr = list.begin();
    repeatMin = *itr++;
    repeatHour = *itr++;
    repeatDayOfMonth = *itr++;
    repeatMonth = *itr++;
    repeatDayOfWeek = *itr;
  } else {
    repeatMin = repeatHour = repeatDayOfMonth = repeatMonth = -1;
    repeatDayOfWeek = 0;
  }
}
