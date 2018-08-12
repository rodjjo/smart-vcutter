/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <boost/filesystem.hpp>

#define BOOST_TEST_MAIN

#include "testing.h"

struct GlobalFixture {
    GlobalFixture() {
        if (!boost::filesystem::exists("data/tmp")) {
            boost::filesystem::create_directory("data/tmp");
        }
    }
    ~GlobalFixture() {  }
};

BOOST_GLOBAL_FIXTURE( GlobalFixture );