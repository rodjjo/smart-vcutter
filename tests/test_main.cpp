/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <boost/filesystem.hpp>

#define BOOST_TEST_MAIN

#include "testing.h"
#include "src/vstream/video_stream.h"

struct GlobalFixture {
    GlobalFixture() {
        vs::initialize();
        if (!boost::filesystem::exists("data/tmp")) {
            boost::filesystem::create_directory("data/tmp");
        }
    }
    ~GlobalFixture() {  }
};

BOOST_GLOBAL_FIXTURE( GlobalFixture );