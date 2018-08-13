#include "tests/testing.h"
#include "src/clippings/clipping_key.h"
#include "src/clippings/clipping.h"


BOOST_AUTO_TEST_SUITE(clipping_key_test_suite)

BOOST_AUTO_TEST_CASE(test_clipping_key_constructors) {
    Json::Value data;
    
    data["frame"] = 120;
    data["px"] = 2;
    data["py"] = 3;
    data["scale"] = 4;
    data["angle"] = 360000;

    vcutter::ClippingKey k1;
    vcutter::ClippingKey k2(data);

    BOOST_CHECK_EQUAL(k1.angle(), 0.0);
    BOOST_CHECK_EQUAL(k1.scale, 1.0);
    BOOST_CHECK_EQUAL(k1.px, 0);
    BOOST_CHECK_EQUAL(k1.py, 0);
    BOOST_CHECK_EQUAL(k1.frame, 0);

    BOOST_CHECK_EQUAL(k2.angle(), 360.0);
    BOOST_CHECK_EQUAL(k2.scale, 4.0);
    BOOST_CHECK_EQUAL(k2.px, 2);
    BOOST_CHECK_EQUAL(k2.py, 3);
    BOOST_CHECK_EQUAL(k2.frame, 120);
}

BOOST_AUTO_TEST_CASE(test_clipping_key_serialize) {
    vcutter::ClippingKey k1;

    k1.frame = 120;
    k1.px = 2;
    k1.py = 3;
    k1.scale = 4;
    k1.angle(360);

    Json::Value data = k1.serialize();

    BOOST_CHECK_EQUAL(data["frame"].asInt(), 120);
    BOOST_CHECK_EQUAL(data["px"].asInt(), 2);
    BOOST_CHECK_EQUAL(data["py"].asInt(), 3);
    BOOST_CHECK_EQUAL(data["scale"].asFloat(), 4.0);
    BOOST_CHECK_EQUAL(data["angle"].asInt(), 360000);
}

BOOST_AUTO_TEST_CASE(test_clipping_key_angle) {
    vcutter::ClippingKey k1;
    vcutter::ClippingKey k2;
    vcutter::ClippingKey k3;
    k1.angle(350.124);
    k2.angle(370);
    k3.angle(-20);
    BOOST_CHECK_CLOSE(k1.angle(), 350.124, 0.1);
    BOOST_CHECK_EQUAL(k2.angle(), 10);
    BOOST_CHECK_EQUAL(k3.angle(), 340);
}

BOOST_AUTO_TEST_SUITE_END()
