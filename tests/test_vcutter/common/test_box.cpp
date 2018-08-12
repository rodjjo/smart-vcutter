#include "tests/testing.h"
#include "src/common/box.h"

BOOST_AUTO_TEST_SUITE(box_test_suite)

BOOST_AUTO_TEST_CASE(test_box_constructors) {
    vcutter::point_t p[4] = {
        vcutter::point_t(1, 2), 
        vcutter::point_t(3, 4), 
        vcutter::point_t(5, 6), 
        vcutter::point_t(7, 8)
    };

    vcutter::box_t b1;
    vcutter::box_t b2(p[0], p[1], p[2], p[3]);

    BOOST_CHECK_EQUAL(b2[0].x, p[0].x);
    BOOST_CHECK_EQUAL(b2[0].y, p[0].y);
    BOOST_CHECK_EQUAL(b2[1].x, p[1].x);
    BOOST_CHECK_EQUAL(b2[1].y, p[1].y);
    BOOST_CHECK_EQUAL(b2[2].x, p[2].x);
    BOOST_CHECK_EQUAL(b2[2].y, p[2].y);
    BOOST_CHECK_EQUAL(b2[3].x, p[3].x);
    BOOST_CHECK_EQUAL(b2[3].y, p[3].y);

    bool box1_all_points_zero = true;

    for (char i = 0; i < 4 && box1_all_points_zero; ++i)
        box1_all_points_zero = (b1[i].x == 0 || b1[i].y == 0);

    BOOST_CHECK(box1_all_points_zero);
}

BOOST_AUTO_TEST_CASE(text_box_rotate) {
    vcutter::point_t p[4] = {
        vcutter::point_t(-10, -10), 
        vcutter::point_t(10, -10), 
        vcutter::point_t(10, 10), 
        vcutter::point_t(-10, 10)
    };

    vcutter::box_t b1(p[0], p[1], p[2], p[3]);
    vcutter::box_t b2 = b1.rotated(45);
    b1.rotate(45);

    bool boxes_are_equal = true;

    for (char i = 0; i < 4 && boxes_are_equal; ++i)
        boxes_are_equal = (b1[i].x == b2[i].x) && (b1[i].y == b2[i].y);

    BOOST_CHECK(boxes_are_equal);

    BOOST_CHECK_CLOSE(b2[0].x, 0, 1);
    BOOST_CHECK_CLOSE(b2[0].y, -14, 2);
    BOOST_CHECK_CLOSE(b2[1].x, 14, 2);
    BOOST_CHECK_CLOSE(b2[1].y, 0, 1);
    BOOST_CHECK_CLOSE(b2[2].x, 0, 1);
    BOOST_CHECK_CLOSE(b2[2].y, 14, 2);
    BOOST_CHECK_CLOSE(b2[3].x, -14, 2);
    BOOST_CHECK_CLOSE(b2[3].y, 0, 1);
}

BOOST_AUTO_TEST_CASE(text_box_translate) {
    vcutter::point_t p[4] = {
        vcutter::point_t(5, 10), 
        vcutter::point_t(15, 20), 
        vcutter::point_t(25, 30), 
        vcutter::point_t(35, 40)
    };

    vcutter::box_t b1(p[0], p[1], p[2], p[3]);
    b1.translate(100, 101);

    BOOST_CHECK_EQUAL(b1[0].x, 105);
    BOOST_CHECK_EQUAL(b1[0].y, 111);
    BOOST_CHECK_EQUAL(b1[1].x, 115);
    BOOST_CHECK_EQUAL(b1[1].y, 121);
    BOOST_CHECK_EQUAL(b1[2].x, 125);
    BOOST_CHECK_EQUAL(b1[2].y, 131);
    BOOST_CHECK_EQUAL(b1[3].x, 135);
    BOOST_CHECK_EQUAL(b1[3].y, 141);
}

BOOST_AUTO_TEST_CASE(test_box_scale) {
    vcutter::point_t p[4] = {
        vcutter::point_t(10, 20), 
        vcutter::point_t(30, 40), 
        vcutter::point_t(50, 60), 
        vcutter::point_t(70, 80)
    };

    vcutter::box_t b1(p[0], p[1], p[2], p[3]);
    b1.scale(0.5);

    BOOST_CHECK_EQUAL(b1[0].x, 5);
    BOOST_CHECK_EQUAL(b1[0].y, 10);
    BOOST_CHECK_EQUAL(b1[1].x, 15);
    BOOST_CHECK_EQUAL(b1[1].y, 20);
    BOOST_CHECK_EQUAL(b1[2].x, 25);
    BOOST_CHECK_EQUAL(b1[2].y, 30);
    BOOST_CHECK_EQUAL(b1[3].x, 35);
    BOOST_CHECK_EQUAL(b1[3].y, 40);
}

BOOST_AUTO_TEST_CASE(test_box_center) {
    vcutter::point_t p[4] = {
        vcutter::point_t(10, 12), 
        vcutter::point_t(20, 12), 
        vcutter::point_t(20, 22), 
        vcutter::point_t(10, 22)
    };

    vcutter::box_t b1(p[0], p[1], p[2], p[3]);
    auto center = b1.center();

    BOOST_CHECK_EQUAL(center.x, 15);
    BOOST_CHECK_EQUAL(center.y, 17);
}

BOOST_AUTO_TEST_CASE(test_box_size) {
    // the box must not be rotated
    vcutter::point_t p[4] = {
        vcutter::point_t(-10, -20), 
        vcutter::point_t(10, -20), 
        vcutter::point_t(10, 20), 
        vcutter::point_t(-10, 20)
    };

    vcutter::box_t b1(p[0], p[1], p[2], p[3]);
    auto size = b1.size();

    BOOST_CHECK_EQUAL(size.x, 20);
    BOOST_CHECK_EQUAL(size.y, 40);
}

BOOST_AUTO_TEST_CASE(test_box_occupied_area) {
    vcutter::point_t p[4] = {
        vcutter::point_t(-10, -20), 
        vcutter::point_t(20, 20), 
        vcutter::point_t(30, 60), 
        vcutter::point_t(40, 100)
    };

    vcutter::box_t box(p[0], p[1], p[2], p[3]);
    auto b1 = box.occupied_area();

    BOOST_CHECK_EQUAL(b1[0].x, -10);
    BOOST_CHECK_EQUAL(b1[0].y, -20);
    BOOST_CHECK_EQUAL(b1[1].x, 40);
    BOOST_CHECK_EQUAL(b1[1].y, -20);
    BOOST_CHECK_EQUAL(b1[2].x, 40);
    BOOST_CHECK_EQUAL(b1[2].y, 100);
    BOOST_CHECK_EQUAL(b1[3].x, -10);
    BOOST_CHECK_EQUAL(b1[3].y, 100);
}


BOOST_AUTO_TEST_CASE(test_box_trunc_precision) {
    vcutter::point_t p[4] = {
        vcutter::point_t(1.9, 2.9), 
        vcutter::point_t(3.9, 4.9), 
        vcutter::point_t(5.9, 6.9), 
        vcutter::point_t(7.9, 8.9)
    };

    vcutter::box_t b1(p[0], p[1], p[2], p[3]);
    b1.trunc_precision();

    BOOST_CHECK_EQUAL(b1[0].x, 1);
    BOOST_CHECK_EQUAL(b1[0].y, 2);
    BOOST_CHECK_EQUAL(b1[1].x, 3);
    BOOST_CHECK_EQUAL(b1[1].y, 4);
    BOOST_CHECK_EQUAL(b1[2].x, 5);
    BOOST_CHECK_EQUAL(b1[2].y, 6);
    BOOST_CHECK_EQUAL(b1[3].x, 7);
    BOOST_CHECK_EQUAL(b1[3].y, 8);
}

BOOST_AUTO_TEST_CASE(test_box_coordinates_violation) {
    // the box must not be rotated
    vcutter::point_t p[4] = {
        vcutter::point_t(-10, -20), 
        vcutter::point_t(15, -20), 
        vcutter::point_t(15, 20), 
        vcutter::point_t(-10, 20)
    };

    vcutter::box_t b1(p[0], p[1], p[2], p[3]);
    auto left_top_violation = b1.left_top_violation(10, 10);
    auto right_bottom_violation = b1.right_bottom_violation(10, 10);
    
    BOOST_CHECK_EQUAL(left_top_violation.x, 10);
    BOOST_CHECK_EQUAL(left_top_violation.y, 20);
    BOOST_CHECK_EQUAL(right_bottom_violation.x, 5);
    BOOST_CHECK_EQUAL(right_bottom_violation.y, 10);
}

BOOST_AUTO_TEST_CASE(test_box_countours_point) {
    vcutter::point_t p[4] = {
        vcutter::point_t(4, -4), 
        vcutter::point_t(4, 4), 
        vcutter::point_t(-4, 4), 
        vcutter::point_t(-4, -4)
    };

    vcutter::point_t p2[4] = {
        vcutter::point_t(0, -10), 
        vcutter::point_t(10, 0), 
        vcutter::point_t(0, 10), 
        vcutter::point_t(-10, 0)
    };

    vcutter::point_t p3[4] = {
        vcutter::point_t(6, -6), 
        vcutter::point_t(6, 6), 
        vcutter::point_t(-6, 6), 
        vcutter::point_t(-6, -6)
    };

    vcutter::box_t b(p2[0], p2[1], p2[2], p2[3]);

    char i;
    for (char c = 0; c < 4; ++c) {
        i = (c + 1) % 4;
        p[c].x = (p[c].x + p[i].x) / 2;
        p[c].y = (p[c].y + p[i].y) / 2;
    }

    BOOST_CHECK(b.contours_point(p[0]));
    BOOST_CHECK(b.contours_point(p[1]));
    BOOST_CHECK(b.contours_point(p[2]));
    BOOST_CHECK(b.contours_point(p[3]));

    BOOST_CHECK(b.contours_point(p[0].x, p[0].y));
    BOOST_CHECK(b.contours_point(p[1].x, p[1].y));
    BOOST_CHECK(b.contours_point(p[2].x, p[2].y));
    BOOST_CHECK(b.contours_point(p[3].x, p[3].y));

    BOOST_CHECK(!b.contours_point(p3[0]));
    BOOST_CHECK(!b.contours_point(p3[1]));
    BOOST_CHECK(!b.contours_point(p3[2]));
    BOOST_CHECK(!b.contours_point(p3[3]));

    BOOST_CHECK(!b.contours_point(p3[0].x, p3[0].y));
    BOOST_CHECK(!b.contours_point(p3[1].x, p3[1].y));
    BOOST_CHECK(!b.contours_point(p3[2].x, p3[2].y));
    BOOST_CHECK(!b.contours_point(p3[3].x, p3[3].y));
}

BOOST_AUTO_TEST_SUITE_END()