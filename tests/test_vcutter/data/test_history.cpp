#include <cstdio>
#include "tests/testing.h"
#include "src/data/history.h"

BOOST_AUTO_TEST_SUITE(test_history)

BOOST_AUTO_TEST_CASE(test_history_get_set) {
    const char * temp_path = "data/tmp/history_temp.json";

    vcutter::History h1(temp_path);
    h1.set("history", "works");

    vcutter::History h2(temp_path);
    BOOST_CHECK_EQUAL(h2["history"], "works");

    h2.set("history", "works allways");
    vcutter::History h3(temp_path);

    BOOST_CHECK_EQUAL(h3["history"], "works allways");
    BOOST_CHECK_EQUAL(h2["history"], "works allways");
    BOOST_CHECK_EQUAL(h1["history"], "works");

    std::remove(temp_path);
}

BOOST_AUTO_TEST_SUITE_END()