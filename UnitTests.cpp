#ifdef TEST
#include <QApplication>
#include <QAction>
#include <QFile>
#include <gtest/gtest.h>
#include "speaker.h"
#include "hourreader.h"
#include "qsnapper.h"

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
    currentTime->tm_min=0;
    currentTime->tm_sec=0;
    currentTime->tm_min=0;

    ASSERT_EQ(mktime(currentTime), hr.nextCheckTime().toTime_t());
}

TEST(HourReaderTests, HourReaderGetTextWorks)
{
    HourReader hr(nullptr);

    auto currentTime_t = time(NULL);
    tm *currentTime = localtime(&currentTime_t);

    ASSERT_EQ("The time is now " + QString::number(currentTime->tm_hour) + " hundred hours", hr.getText());
}

TEST(QSnapperTests, QSnapperStartsMuted)
{
    QSnapper Snapper(nullptr);
    ASSERT_TRUE(Snapper.isMuted());
}

TEST(QSnapperTests, QSnapperHasThreeMenuItems)
{
    QSnapper Snapper(nullptr);
    ASSERT_EQ(3, Snapper.getMenuContents().size());
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
    ASSERT_EQ(QDateTime::fromTime_t(0),Snapper.nextCheckTime());
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

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}

#endif
