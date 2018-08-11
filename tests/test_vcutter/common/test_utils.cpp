#include <fstream>
#include <algorithm>
#include <string> 
#include <boost/filesystem.hpp>
#include "tests/testing.h"
#include "smart-vcutter/common/utils.h"

BOOST_AUTO_TEST_SUITE(utils_test_suite)

BOOST_AUTO_TEST_CASE(test_change_filepath_dir) {
    BOOST_CHECK_EQUAL(vcutter::change_filepath_dir("./teste.mp4", "../", ".webm", false), "../teste.webm");
    BOOST_CHECK_EQUAL(vcutter::change_filepath_dir("./teste.mp4", "../", ".webm", true), "../teste.001.webm");
    BOOST_CHECK_EQUAL(
        boost::filesystem::path(vcutter::change_filepath_dir("./some_where/test_file_name.mp4", "./data", ".txt", true)),
        boost::filesystem::path("./data/test_file_name.002.txt"));
}

BOOST_AUTO_TEST_CASE(test_filepath_exists) {
    BOOST_CHECK(vcutter::filepath_exists("./data/test_file_name.001.txt"));
    BOOST_CHECK(!vcutter::filepath_exists("./data/test_file_name.002.txt"));
}

BOOST_AUTO_TEST_CASE(test_generate_path) {
    BOOST_CHECK_EQUAL(
        boost::filesystem::path(vcutter::generate_path("./data/test_file_name.", ".txt", 1)),
        boost::filesystem::path("./data/test_file_name.002.txt"));
    BOOST_CHECK_EQUAL(
        boost::filesystem::path(vcutter::generate_path("./data/test_file_name.", ".txt", 3)),
        boost::filesystem::path("./data/test_file_name.003.txt"));
    BOOST_CHECK_EQUAL(
        boost::filesystem::path(vcutter::generate_path("./data/teste_other_file.", ".txt", 1)),
        boost::filesystem::path("./data/teste_other_file.001.txt"));
}

BOOST_AUTO_TEST_CASE(test_remove_file) {
    const char *existing_path = "./data/file_to_remove_vcutter.test";
    {
        std::ofstream ofile;
        ofile.open(existing_path);
        ofile << "this file must be removed";
    } // ofile scope

    BOOST_CHECK(vcutter::remove_file(existing_path));
    BOOST_CHECK(vcutter::remove_file("./data/this_path_does_not_exists.txt"));
    BOOST_CHECK(!boost::filesystem::exists(existing_path));
}

BOOST_AUTO_TEST_CASE(test_str_to_milliseconds) {
    BOOST_CHECK_EQUAL(vcutter::str_to_milliseconds("01:00:00"), 3600000);
    BOOST_CHECK_EQUAL(vcutter::str_to_milliseconds("00:01:00"), 60000);
    BOOST_CHECK_EQUAL(vcutter::str_to_milliseconds("00:00:01"), 1000);
    BOOST_CHECK_EQUAL(vcutter::str_to_milliseconds("01:01:01"), 3661000);
    BOOST_CHECK_EQUAL(vcutter::str_to_milliseconds("01:01:01,009"), 3661009);
    BOOST_CHECK_EQUAL(vcutter::str_to_milliseconds("59:59"), 3599000);
    BOOST_CHECK_EQUAL(vcutter::str_to_milliseconds(""), 0);
}

BOOST_AUTO_TEST_CASE(test_temp_filepath) {
    const char *filename = "vcutter_test.txt";
    std::string temp_file = vcutter::temp_filepath(filename);
    std::transform(temp_file.begin(), temp_file.end(), temp_file.begin(), ::tolower);

#ifdef WIN32
    BOOST_CHECK(temp_file.find("temp") != std::string::npos);
#else
    BOOST_CHECK(temp_file.find("tmp") != std::string::npos);
#endif

    BOOST_CHECK(temp_file.find(filename) != std::string::npos);
}

BOOST_AUTO_TEST_CASE(test_seconds_to_str) {
    char temp_buffer[30];

    vcutter::seconds_to_str(temp_buffer, 30, 3600);
    BOOST_CHECK_EQUAL(temp_buffer, std::string("01:00:00"));
    
    vcutter::seconds_to_str(temp_buffer, 30, 60);
    BOOST_CHECK_EQUAL(temp_buffer, std::string("00:01:00"));
    
    vcutter::seconds_to_str(temp_buffer, 30, 1);
    BOOST_CHECK_EQUAL(temp_buffer, std::string("00:00:01"));

    vcutter::seconds_to_str(temp_buffer, 30, 3661);
    BOOST_CHECK_EQUAL(temp_buffer, std::string("01:01:01"));
    
    vcutter::seconds_to_str(temp_buffer, 30, 3661.009, true);
    BOOST_CHECK_EQUAL(temp_buffer, std::string("01:01:01,009"));
}

BOOST_AUTO_TEST_SUITE_END()