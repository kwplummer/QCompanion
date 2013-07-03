#ifdef TEST
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <time.h>
#include "component.h"
#include "speaker.h"

BOOST_AUTO_TEST_SUITE(SpeakerSuite)

BOOST_AUTO_TEST_CASE(CanInvokeFlite)
{
    Speaker s;
    s.speak("");
    BOOST_CHECK(s.finishSpeaking());
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Libs)

BOOST_AUTO_TEST_CASE(LoadingBadComponentFails)
{
    BOOST_CHECK_THROW(loadComponent("null"),ComponentException)
}

BOOST_AUTO_TEST_CASE(LoadingMockComponentWorks)
{
    Component *mock = loadComponent("./libmock.so");
    delete mock;
}

BOOST_AUTO_TEST_CASE(CanCheckMockNextTime)
{
    Component *mock = loadComponent("./libmock.so");
    BOOST_CHECK(mock->nextCheckTime() == 5);
    delete mock;
}

BOOST_AUTO_TEST_CASE(MockHasRightText)
{
    Component *mock = loadComponent("./libmock.so");
    BOOST_CHECK(mock->getText() == "this is a test.");
}

BOOST_AUTO_TEST_CASE(MockCanSpeak)
{
    Speaker s;
    Component *mock = loadComponent("./libmock.so");
    s.speak(mock->getText().c_str());
    BOOST_CHECK(s.finishSpeaking());
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(HourReader)

BOOST_AUTO_TEST_CASE(CanLoadHourReader)
{
    Component *hourReader = loadComponent("./libhourreader.so");
    delete hourReader;
}

BOOST_AUTO_TEST_CASE(HourReaderCorrectlyReportsNextHour)
{
    Component *hourReader = loadComponent("./libhourreader.so");
    auto currentTime_t = time(NULL);
    tm *currentTime = localtime(&currentTime_t);
    ++currentTime->tm_hour;
    currentTime->tm_min=0;
    currentTime->tm_sec=0;
    currentTime->tm_min=0;
    BOOST_CHECK(mktime(currentTime) == hourReader->nextCheckTime());
    delete hourReader;
}

BOOST_AUTO_TEST_CASE(HourReaderHasRightText)
{
    Component *hourReader = loadComponent("./libhourreader.so");
    auto currentTime_t = time(NULL);
    tm *currentTime = localtime(&currentTime_t);
    auto ComparisonString =     "The time is now " +
                                (currentTime->tm_hour > 10 ? std::to_string(currentTime->tm_hour) : '0' + std::to_string(currentTime->tm_hour))
                                + " hundred hours";
    BOOST_CHECK(ComparisonString == hourReader->getText());
}

BOOST_AUTO_TEST_SUITE_END()

#endif
