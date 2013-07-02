#ifdef TEST
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include "component.h"

{
BOOST_AUTO_TEST_SUITE(Libs)

BOOST_AUTO_TEST_CASE(LoadingBadComponentFails)
{
    BOOST_CHECK_THROW(loadComponent("null"),ComponentException)
}

BOOST_AUTO_TEST_CASE(LoadingMockComponentWorks)
{
    Component *mock = loadComponent("./mock.so");
    delete mock;
}

BOOST_AUTO_TEST_CASE(CanCheckMockNextTime)
{
    Component *mock = loadComponent("./mock.so");
    BOOST_CHECK(mock->nextCheckTime() == 5);
    delete mock;
}

BOOST_AUTO_TEST_SUITE_END()
}

#endif
