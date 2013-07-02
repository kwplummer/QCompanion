#ifdef TEST
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <time.h>
#include "component.h"

BOOST_AUTO_TEST_SUITE(SpeakerSuite)

BOOST_AUTO_TEST_CASE(CanInvokeFlite)
{
    Speaker s;
    s.speak("");
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

BOOST_AUTO_TEST_CASE(MockCanSpeak)
{
    Speaker s;
    Component *mock = loadComponent("./libmock.so");
    mock->speak(s);
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
    Speaker s;
    delete hourReader;
}

BOOST_AUTO_TEST_SUITE_END()

#endif
