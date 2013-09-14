#ifdef TEST
#include <QApplication>
#include <QAction>
#include <QFile>
#include <gtest/gtest.h>
#include "speaker.h"
#include "hourreader.h"
#include "qsnapper.h"
#include "waitercrondialog.h"
#include "waiterwidget.h"
// QDateTime::fromTime_t(1234567890) is Fri Feb 13 15:31:30 2009 PST

TEST(ComponentTests, HourReaderStartsUnmuted)
{
  HourReader hr(nullptr);
  ASSERT_FALSE(hr.isMuted());
}

TEST(ComponentTests, SetMuteWorks)
{
  HourReader hr(nullptr);
  hr.getMenuContents().at(0)->trigger();
  ASSERT_TRUE(hr.isMuted());
  hr.getMenuContents().at(0)->trigger();
  ASSERT_FALSE(hr.isMuted());
}

TEST(HourReaderTests, HourReaderNextCheckTimeWorks)
{
  HourReader hr(nullptr);

  auto currentTime_t = time(NULL);
  tm *currentTime = localtime(&currentTime_t);
  ++currentTime->tm_hour;
  currentTime->tm_min = 0;
  currentTime->tm_sec = 0;
  currentTime->tm_min = 0;

  ASSERT_EQ(mktime(currentTime), hr.nextCheckTime().toTime_t());
}

TEST(HourReaderTests, HourReaderGetTextWorks)
{
  HourReader hr(nullptr);

  auto currentTime_t = time(NULL);
  tm *currentTime = localtime(&currentTime_t);

  QString hour = currentTime->tm_hour > 10
                     ? "" + QString::number(currentTime->tm_hour)
                     : "0" + QString::number(currentTime->tm_hour);
  QString test = "The time is now " + hour + " hundred hours";
  ASSERT_EQ("The time is now " + hour + " hundred hours", hr.getText());
}

TEST(QSnapperTests, QSnapperStartsMuted)
{
  QSnapper Snapper(nullptr);
  ASSERT_TRUE(Snapper.isMuted());
}

TEST(QSnapperTests, QSnapperHasFiveMenuItems)
{
  QSnapper Snapper(nullptr);
  ASSERT_EQ(5, Snapper.getMenuContents().size());
}

TEST(QSnapperTests, QSnapperHasMuteActionChecked)
{
  QSnapper Snapper(nullptr);
  ASSERT_TRUE(Snapper.getMenuContents().at(1)->isChecked());
}

TEST(QSnapperTests, QSnapperHasNoNextTimeWhenDisabled)
{
  QSnapper Snapper(nullptr);
  bool wasEnabled = Snapper.isEnabled();
  Snapper.enableSnapping(false);
  ASSERT_EQ(QDateTime::fromTime_t(0), Snapper.nextCheckTime());
  Snapper.setEnabled(wasEnabled);
}

TEST(QSnapperTests, QSnapperNextSnapIsSixtySecondsLaterWhenEnabled)
{
  QSnapper Snapper(nullptr);
  bool wasEnabled = Snapper.isEnabled();
  Snapper.enableSnapping(true);
  QDateTime nextTime = QDateTime::currentDateTime().addSecs(60);
  ASSERT_EQ(nextTime.toTime_t(), Snapper.nextCheckTime().toTime_t());
  Snapper.setEnabled(wasEnabled);
}

TEST(QSnapperTests, QSnapperSaysSnap)
{
  QSnapper Snapper(nullptr);
  ASSERT_EQ("Snap", Snapper.getText());
}

TEST(QSnapperTests, QSnapperHasChangeSaveLocationMenuAction)
{
  QSnapper Snapper(nullptr);
  ASSERT_EQ("Change Save Location", Snapper.getMenuContents().at(0)->text());
}

TEST(SpeakerTests, SpeakerStartsUnmuted)
{
  Speaker s("");
  ASSERT_TRUE(s.isTTSEnabled());
}

TEST(SpeakerTests, SpeakerStartsWithNotificationsOn)
{
  Speaker s("");
  ASSERT_TRUE(s.isNotificationsEnabled());
}

TEST(SpeakerTests, SpeakerCanBeMuted)
{
  Speaker s("");
  s.setTTSEnabled(false);
  ASSERT_FALSE(s.isTTSEnabled());
}

TEST(SpeakerTests, SpeakerCanDisableNotifications)
{
  Speaker s("");
  s.setNotificationsEnabled(false);
  ASSERT_FALSE(s.isNotificationsEnabled());
}

TEST(SpeakerTests, SpeakerCanSpeak)
{
  Speaker s("");
  s.speak("");
}

TEST(WaiterCronOccuranceTests, EmptyInitializerListSetsValuesToDefault)
{
  WaiterCronOccurance repeat{};
  ASSERT_EQ(-1, repeat.repeatMin);
  ASSERT_EQ(-1, repeat.repeatHour);
  ASSERT_EQ(-1, repeat.repeatDayOfMonth);
  ASSERT_EQ(-1, repeat.repeatMonth);
  ASSERT_EQ(0, repeat.repeatDayOfWeek);
}

TEST(WaiterCronOccuranceTests,
     InitializerListWithLessItemsThanNeededSetsValuesToDefault)
{
  WaiterCronOccurance repeat{ 8, 12 };
  ASSERT_EQ(-1, repeat.repeatMin);
  ASSERT_EQ(-1, repeat.repeatHour);
  ASSERT_EQ(-1, repeat.repeatDayOfMonth);
  ASSERT_EQ(-1, repeat.repeatMonth);
  ASSERT_EQ(0, repeat.repeatDayOfWeek);
}

TEST(WaiterCronOccuranceTests,
     InitializerListWithRequiredItemsSetsValuesCorrectly)
{
  WaiterCronOccurance repeat{ 1, 2, 3, 4, 5 };
  ASSERT_EQ(1, repeat.repeatMin);
  ASSERT_EQ(2, repeat.repeatHour);
  ASSERT_EQ(3, repeat.repeatDayOfMonth);
  ASSERT_EQ(4, repeat.repeatMonth);
  ASSERT_EQ(5, repeat.repeatDayOfWeek);
}

TEST(WaiterCronOccuranceTests,
     InitializerListWithLessItemsThanNeededSetsValuesCorrectly)
{
  WaiterCronOccurance repeat{ 1, 2, 3, 4, 5, 6, 7, 8 };
  ASSERT_EQ(1, repeat.repeatMin);
  ASSERT_EQ(2, repeat.repeatHour);
  ASSERT_EQ(3, repeat.repeatDayOfMonth);
  ASSERT_EQ(4, repeat.repeatMonth);
  ASSERT_EQ(5, repeat.repeatDayOfWeek);
}

TEST(WaiterCronOccuranceTests, CopyConstructionFromDefaultOccuranceWorks)
{
  WaiterCronOccurance repeat{};
  WaiterCronOccurance clone{ repeat };
  ASSERT_EQ(-1, clone.repeatMin);
  ASSERT_EQ(-1, clone.repeatHour);
  ASSERT_EQ(-1, clone.repeatDayOfMonth);
  ASSERT_EQ(-1, clone.repeatMonth);
  ASSERT_EQ(0, clone.repeatDayOfWeek);
}

TEST(WaiterCronOccuranceTests, CopyConstructionFromCustomOccuranceWorks)
{
  WaiterCronOccurance repeat{ 1, 2, 3, 4, 5 };
  WaiterCronOccurance clone{ repeat };
  ASSERT_EQ(1, clone.repeatMin);
  ASSERT_EQ(2, clone.repeatHour);
  ASSERT_EQ(3, clone.repeatDayOfMonth);
  ASSERT_EQ(4, clone.repeatMonth);
  ASSERT_EQ(5, clone.repeatDayOfWeek);
}

TEST(WaiterCronOccuranceTests, DefaultOccurancesDoNotRepeat)
{
  WaiterCronOccurance repeat{};
  ASSERT_FALSE(repeat.repeats());
}

TEST(WaiterCronOccuranceTests, CustomOccurancesDoRepeat)
{
  WaiterCronOccurance repeat{ 1, 2, 3, 4, 5 };
  ASSERT_TRUE(repeat.repeats());
}

TEST(WaiterCronOccuranceTests, NextOccuranceOnADefaultItemIsZero)
{
  WaiterCronOccurance repeat{};
  QDateTime now = QDateTime::fromTime_t(1234567890);
  ASSERT_EQ(QDateTime::fromTime_t(0), repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustMinAfterNowWorks)
{
  WaiterCronOccurance repeat{ 57, -1, -1, -1, 0 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(testTime.hour(), 57, 0);
  QDateTime test(now.date(), testTime);
  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustMinBeforeNowWorks)
{
  WaiterCronOccurance repeat{ 2, -1, -1, -1, 0 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(testTime.hour() + 1, 2, 0);
  QDateTime test(now.date(), testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustHourAfterNowWorks)
{
  QDateTime now = QDateTime::fromTime_t(1234567890);
  const short hourAdjust = (now.time().hour() + 3);
  WaiterCronOccurance repeat{ -1, (hourAdjust % 24), -1, -1, 0 };

  QDateTime test(now);
  test = test.addSecs(3 * 60 * 60);
  test = test.addSecs(-test.time().second() - test.time().minute() * 60);
  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustHourBeforeNowWorks)
{
  QDateTime now = QDateTime::fromTime_t(1234567890);
  int hourAdjust = (now.time().hour() - 3);
  if(hourAdjust < 0)
    hourAdjust += 24;
  WaiterCronOccurance repeat{ -1, (short)hourAdjust, -1, -1, 0 };

  QDateTime test(now);
  test = test.addSecs(-3 * 60 * 60);
  test = test.addDays(1);
  test = test.addSecs(-test.time().second() - test.time().minute() * 60);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustDayOfMonthAfterNowWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 15, -1, 0 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 15);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustDayOfMonthBeforeNowWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 5, -1, 0 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 3, 5);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustMonthAfterNowWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, 7, 0 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 7, 1);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustMonthBeforeNowWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, 1, 0 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2010, 1, 1);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustMondayWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, -1, 1 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 16);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustTuesdayWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, -1, 2 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 17);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustWednesdayWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, -1, 3 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 18);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustThursdayWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, -1, 4 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 19);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustFridayWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, -1, 5 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 20);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustSaturdayWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, -1, 6 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 14);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithJustSundayWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, -1, 7 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 15);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithMinAndHourBeforeWorks)
{
  QDateTime now = QDateTime::fromTime_t(1234567890);
  short hourAdjust = now.time().hour() - 6;
  if(hourAdjust < 0)
    hourAdjust += 24;
  WaiterCronOccurance repeat{ 9, hourAdjust, -1, -1, 0 };
  QDateTime test(now);
  test = test.addSecs(-6 * 60 * 60);
  test = test.addDays(1);
  test = test.addSecs(-test.time().second() - test.time().minute() * 60);
  test = test.addSecs(9 * 60);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithMinAndHourAfterWorks)
{
  QDateTime now = QDateTime::fromTime_t(1234567890);
  const short hourAjust = now.time().hour() + 4;
  WaiterCronOccurance repeat{ 5, (hourAjust % 24), -1, -1, 0 };
  QDateTime test(now);
  test = test.addSecs(4 * 60 * 60);
  test = test.addSecs(-test.time().second() - test.time().minute() * 60);
  test = test.addSecs(5 * 60);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithMonthAndDayOfMonthBeforeWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 5, 1, 0 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2010, 1, 5);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithMonthAndDayOfMonthAfterWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 5, 7, 0 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 7, 5);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithMonthAndDayOfWeekBeforeWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, 1, 6 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2010, 1, 2);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithMonthAndDayOfWeekAfterWorks)
{
  WaiterCronOccurance repeat{ -1, -1, -1, 5, 7 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 5, 3);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithDayOfWeekBeforeDayOfMonthWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 10, -1, 5 };
  QDateTime now = QDateTime::fromTime_t(256);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(1970, 1, 2);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, NextOccuranceWithDayOfWeekAfterDayOfMonthWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 14, -1, 7 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 2, 14);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests,
     NextOccuranceWithDayOfWeekBeforeDayOfMonthAndMonthBeforeNowWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 20, 1, 2 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2010, 1, 5);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests,
     NextOccuranceWithDayOfWeekBeforeDayOfMonthAndMonthAfterNowWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 20, 4, 6 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 4, 4);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests,
     NextOccuranceWithDayOfWeekAfterDayOfMonthAndMonthBeforeNowWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 3, 1, 3 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2010, 1, 3);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests,
     NextOccuranceWithDayOfWeekAfterDayOfMonthAndMonthAfterNowWorks)
{
  WaiterCronOccurance repeat{ -1, -1, 4, 4, 1 };
  QDateTime now = QDateTime::fromTime_t(1234567890);
  QTime testTime = now.time();
  testTime.setHMS(0, 0, 0);
  QDate testDate = now.date();
  testDate.setYMD(2009, 4, 4);
  QDateTime test(testDate, testTime);

  ASSERT_EQ(test, repeat.nextOccurance(now));
}

TEST(WaiterCronOccuranceTests, DefaultToStringWorks)
{
  WaiterCronOccurance repeat{};
  ASSERT_EQ("-1 -1 -1 -1 0", repeat.toString());
}

TEST(WaiterCronOccuranceTests, CustomToStringWorks)
{
  WaiterCronOccurance repeat{ 1, 2, 3, 4, 5 };
  ASSERT_EQ("1 2 3 4 5", repeat.toString());
}

TEST(WaiterCronDialogTests, GetResultReturnsDefaultValuesByDefault)
{
  WaiterCronDialog dialog;
  ASSERT_EQ("-1 -1 -1 -1 0", dialog.getResult().toString());
}

TEST(WaiterCronDialogTests, SetResultFollowedByGetResultWorks)
{
  WaiterCronOccurance repeat{ 1, 2, 3, 4, 5 };
  WaiterCronDialog dialog;
  dialog.setResult(repeat);
  ASSERT_EQ(repeat.toString(), dialog.getResult().toString());
}

TEST(WaiterCronDialogTests, OKIsFalseByDefault)
{
  WaiterCronDialog dialog;
  ASSERT_FALSE(dialog.okWasPressed());
}

class WaiterWidgetTests : public ::testing::Test
{
protected:
  QDateTime test;
  double range;
  virtual void SetUp() override
  {
    test = QDateTime::fromTime_t(1234567890);
    range = 1000;
  }
};

TEST_F(WaiterWidgetTests, MSecsIsAccurate)
{
  WaiterWidget w(nullptr, test.date(), test.time(), "", WaiterCronOccurance{});
  ASSERT_EQ(1234567890000, w.getMsecs());
}

TEST_F(WaiterWidgetTests, ToStringWithNoTitleWorks)
{
  WaiterWidget w(nullptr, test.date(), test.time(), "", WaiterCronOccurance{});
  ASSERT_EQ("NULL 1234567890000 -1 -1 -1 -1 0", w.toLoggableString());
}

TEST_F(WaiterWidgetTests, ToStringWithTitleWorks)
{
  WaiterWidget w(nullptr, test.date(), test.time(), "Title",
                 WaiterCronOccurance{});
  ASSERT_EQ("Title 1234567890000 -1 -1 -1 -1 0", w.toLoggableString());
}

TEST_F(WaiterWidgetTests, ToStringWithTitleWithSpacesWorks)
{
  WaiterWidget w(nullptr, test.date(), test.time(), "Title With Spaces",
                 WaiterCronOccurance{});
  ASSERT_EQ("Title_With_Spaces 1234567890000 -1 -1 -1 -1 0",
            w.toLoggableString());
}

int main(int argc, char **argv)
{
  QApplication a(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#endif
