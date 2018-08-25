/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <memory>

#include "tests/testing.h"
#include "src/clippings/clipping_key.h"
#include "src/clippings/clipping.h"

namespace {

std::unique_ptr<vcutter::Clipping> clp;

class SuiteFixture {
 public:
    SuiteFixture() {
        clp.reset(new vcutter::Clipping("data/sample_video.webm", true));
        clp->w(clp->player()->info()->w());
        clp->h(clp->player()->info()->h());
    }

    ~SuiteFixture() {
        clp.reset();
    }
};

class RestoreClp {
 public:
    ~RestoreClp() {
        clp->w(clp->player()->info()->w());
        clp->h(clp->player()->info()->h());
    }
};

}  // namespace


BOOST_FIXTURE_TEST_SUITE(clipping_key_test_suite, SuiteFixture)


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

BOOST_AUTO_TEST_CASE(test_clipping_key_y_constraints) {
    vcutter::ClippingKey k1;
    k1.scale = 1;
    k1.px = clp->player()->info()->w() / 2;
    k1.py = clp->player()->info()->h() / 4;

    auto k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.5, 1);

    k1.py /= 2;
    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.25, 1);
}

BOOST_AUTO_TEST_CASE(test_clipping_key_individual_x_constraints) {
    vcutter::ClippingKey k1;
    k1.scale = 1;
    k1.px = clp->player()->info()->w() / 4;
    k1.py = clp->player()->info()->h() / 2;

    auto k2 = k1.constrained(clp.get());

    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.5, 1);

    k1.px /= 2;
    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.246, 1);
}


BOOST_AUTO_TEST_CASE(test_clipping_key_individual_xy_constraints) {
    vcutter::ClippingKey k1;
    k1.scale = 1;
    k1.px = clp->player()->info()->w() / 8;
    k1.py = clp->player()->info()->h() / 4;

    auto k2 = k1.constrained(clp.get());

    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.2463, 1);
}

BOOST_AUTO_TEST_CASE(test_clipping_key_individual_scaled_constraints) {
    RestoreClp restore;

    vcutter::ClippingKey k1;
    k1.scale = 5;
    clp->w(200);
    clp->h(350);

    k1.px = clp->player()->info()->w() / 2.0;
    k1.py = clp->player()->info()->h() / 2.0;

    auto k2 = k1.constrained(clp.get());

    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 2, 1);
}

BOOST_AUTO_TEST_CASE(test_clipping_key_individual_rotated_constraints) {
    vcutter::ClippingKey k1;

    k1.angle(90);
    k1.px = clp->player()->info()->w() / 2.0;
    k1.py = clp->player()->info()->h() / 2.0;

    auto k2 = k1.constrained(clp.get());

    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.56111, 1);
}

BOOST_AUTO_TEST_CASE(test_clipping_key_individual_rotscale_constraints) {
    vcutter::ClippingKey k1;

    k1.scale = 4;
    k1.angle(45);
    k1.px = clp->player()->info()->w() / 2.0;
    k1.py = clp->player()->info()->h() / 2.0;

    auto k2 = k1.constrained(clp.get());

    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.5075, 1);
}


BOOST_AUTO_TEST_CASE(test_clipping_key_individual_rotscale_y_constraints) {
    vcutter::ClippingKey k1;

    k1.scale = 4;
    k1.angle(45);
    k1.px = clp->player()->info()->w() / 2.0;
    k1.py = clp->player()->info()->h() / 4.0;

    auto k2 = k1.constrained(clp.get());

    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.4522, 1);
}

BOOST_AUTO_TEST_CASE(test_clipping_key_individual_rotscale_x_constraints) {
    vcutter::ClippingKey k1;

    k1.scale = 4;
    k1.angle(45);
    k1.px = clp->player()->info()->w() / 4.0;
    k1.py = clp->player()->info()->h() / 2.0;

    auto k2 = k1.constrained(clp.get());

    k2 = k1.constrained(clp.get());
    BOOST_CHECK_CLOSE(k2.scale, 0.2537, 1);
}

BOOST_AUTO_TEST_CASE(test_clipping_key_clipping_box) {
    vcutter::ClippingKey k1;

    k1.frame = 120;
    k1.px = 2;
    k1.py = 3;
    k1.scale = 4;
    k1.angle(75);

    vcutter::ClippingKey k2 = k1.constrained(clp.get());

    vcutter::box_t b1 = k1.clipping_box(clp.get());
    vcutter::box_t b2 = k2.clipping_box(clp.get());

    BOOST_CHECK_EQUAL(b1[0].x, 1182);
    BOOST_CHECK_EQUAL(b1[0].y, -1154);
    BOOST_CHECK_EQUAL(b1[1].x, 1603);
    BOOST_CHECK_EQUAL(b1[1].y, 414);
    BOOST_CHECK_EQUAL(b1[2].x, -1178);
    BOOST_CHECK_EQUAL(b1[2].y, 1160);
    BOOST_CHECK_EQUAL(b1[3].x, -1599);
    BOOST_CHECK_EQUAL(b1[3].y, -408);

    BOOST_CHECK_EQUAL(b2[0].x, 3);
    BOOST_CHECK_EQUAL(b2[0].y, 1);
    BOOST_CHECK_EQUAL(b2[1].x, 4);
    BOOST_CHECK_EQUAL(b2[1].y, 3);
    BOOST_CHECK_EQUAL(b2[2].x, 0);
    BOOST_CHECK_EQUAL(b2[2].y, 4);
    BOOST_CHECK_EQUAL(b2[3].x, 0);
    BOOST_CHECK_EQUAL(b2[3].y, 2);
}

BOOST_AUTO_TEST_SUITE_END()
