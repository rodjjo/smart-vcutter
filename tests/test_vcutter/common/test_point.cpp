#include "tests/testing.h"
#include "src/common/point.h"

BOOST_AUTO_TEST_SUITE(point_test_suite)

BOOST_AUTO_TEST_CASE(test_point_constructors) {
    vcutter::point_t p1;
    vcutter::point_t p2(10, 20);

    BOOST_CHECK_EQUAL(p1.x, 0);
    BOOST_CHECK_EQUAL(p1.y, 0);
    BOOST_CHECK_EQUAL(p2.x, 10);
    BOOST_CHECK_EQUAL(p2.y, 20);
}

BOOST_AUTO_TEST_CASE(test_point_to_axis) {
    vcutter::point_t p(10, 20);
    auto p2 = p.to_axis(1, 1);

    BOOST_CHECK_EQUAL(p2.x, 9);
    BOOST_CHECK_EQUAL(p2.y, 19);
}

BOOST_AUTO_TEST_CASE(test_point_rotate) {
    vcutter::point_t p(0, -10);
    p.rotate(45);
    
    BOOST_CHECK_CLOSE(p.x, 7, 2);
    BOOST_CHECK_CLOSE(p.y, -7, 2);
}

BOOST_AUTO_TEST_CASE(test_point_rotated) {
    vcutter::point_t p(0, -10);
    auto p2 = p.rotated(45);
    
    BOOST_CHECK_CLOSE(p2.x, 7, 2);
    BOOST_CHECK_CLOSE(p2.y, -7, 2);
}

BOOST_AUTO_TEST_CASE(test_point_trunc_precision) {
    vcutter::point_t p(1.888, 2.999);
    p.trunc_precision();
    
    BOOST_CHECK_EQUAL(p.x, 1);
    BOOST_CHECK_EQUAL(p.y, 2);
}

BOOST_AUTO_TEST_CASE(test_point_distance_to) {
    vcutter::point_t p(4, 4);
    vcutter::point_t p2(1, 1);

    BOOST_CHECK_CLOSE(p.distance_to(0, 0), 5.65, 1);
    BOOST_CHECK_CLOSE(p.distance_to(p2), 4.24, 1);
}

BOOST_AUTO_TEST_CASE(test_point_angle) {
    vcutter::point_t p(-7, 7);

    BOOST_CHECK_CLOSE(p.angle_0_360(), 45, 1);
    BOOST_CHECK_CLOSE(p.angle(), 45, 1);
}

BOOST_AUTO_TEST_SUITE_END()