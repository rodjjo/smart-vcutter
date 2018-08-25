/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cstdio>
#include <fstream>
#include <boost/filesystem.hpp>
#include "tests/testing.h"
#include "src/data/json_file.h"


BOOST_AUTO_TEST_SUITE(json_file_test_suite)


BOOST_AUTO_TEST_CASE(test_json_file_loads) {
    vcutter::JsonFile jf1("data/not_existing_json.json");
    vcutter::JsonFile jf2("data/test_json_file.json");
    vcutter::JsonFile jf3("data/test_json_broken.json");
    vcutter::JsonFile jf4("data/test_json_file.json", false, false);

    BOOST_CHECK(!jf1.loaded());
    BOOST_CHECK(jf2.loaded());
    BOOST_CHECK(!jf3.loaded());
    BOOST_CHECK(!jf4.loaded());
}

BOOST_AUTO_TEST_CASE(test_json_file_delete) {
    const char *pre_existing_file = "data/tmp/vcutter.json";
    const char *post_existing_file = "data/tmp/vcutter2.json";

    {
        std::ofstream of1(pre_existing_file);
        of1 << "{}";
        of1.close();

        vcutter::JsonFile jf1(pre_existing_file, true);
        vcutter::JsonFile jf2(post_existing_file, true);

        std::ofstream of2(post_existing_file);
        of2 << "{}";
        of2.close();
    }  // JsonFile destructor scope

    BOOST_CHECK(!boost::filesystem::exists(pre_existing_file));
    BOOST_CHECK(boost::filesystem::exists(post_existing_file));

    std::remove(post_existing_file);
}

BOOST_AUTO_TEST_CASE(test_json_file_saves) {
    vcutter::JsonFile jf1("data/tmp/test_json_file.json", true);

    Json::Value root;
    root["that"] = "works";
    jf1.save(root);

    vcutter::JsonFile jf2("data/tmp/test_json_file.json", true);
    BOOST_CHECK_EQUAL(jf2["that"].asString(), "works");
    BOOST_CHECK_EQUAL(jf2.get_data()["that"].asString(), "works");
}

BOOST_AUTO_TEST_SUITE_END()
