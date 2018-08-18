#include <cstdio>
#include <memory>

#include "tests/testing.h"
#include "src/clippings/clipping.h"

namespace {

std::unique_ptr<vcutter::Clipping> clp;

void restore_clipping() {
    clp->remove_all();
    clp->w(80);
    clp->h(180);

    vcutter::ClippingKey k1;

    k1.frame = 120;
    k1.px = 40;
    k1.py = 60;
    k1.scale = 1;
    k1.angle(360);

    clp->add(k1);

    k1.angle(180);
    k1.frame = 1200;
    k1.px = 80;
    k1.py = 120;
    k1.scale = 0.5;

    clp->add(k1);
}

class SuiteFixture {
 public:
    SuiteFixture() {
        clp.reset(new vcutter::Clipping("data/sample_video.webm", true));
        restore_clipping();
    }

    ~SuiteFixture() {
        clp.reset();
    }
};

class ClippingDataStub: public vcutter::ClippingData {
 public:
    ClippingDataStub(): ClippingData("") {
    }

    uint32_t default_w() override {
        return 1280;
    }

    virtual uint32_t default_h() override {
        return 720;
    }

    uint32_t frame_count() override {
        return 10000;
    }
};

class ClippingTearDown {
 public:
    ~ClippingTearDown() {
        restore_clipping();
    }
};

}  // namespace


BOOST_FIXTURE_TEST_SUITE(clipping_test_suite, SuiteFixture)

BOOST_AUTO_TEST_CASE(test_clipping_constructors) {
    vcutter::Clipping clp3("data/it_does_not_exists.mp4", true);

    BOOST_CHECK_EQUAL(clp->good(), true);
    BOOST_CHECK_EQUAL(clp3.good(), false);
}

BOOST_AUTO_TEST_CASE(test_clipping_deserialize) {
    Json::Value data;
    Json::Value key;

    data["video_path"] = "data/sample_video.webm";
    data["width"] = 100;
    data["height"] = 200;

    key["frame"] = 120;
    key["px"] = 2;
    key["py"] = 3;
    key["scale"] = 4;
    key["angle"] = 360000;

    data["keys"].append(key);

    vcutter::Clipping clp2(&data);

    BOOST_CHECK_EQUAL(clp2.good(), true);
    BOOST_CHECK_EQUAL(clp2.w(), 100);
    BOOST_CHECK_EQUAL(clp2.h(), 200);
    BOOST_CHECK_EQUAL(clp2.keys().size(), 1);
}


BOOST_AUTO_TEST_CASE(test_clipping_serialize) {
    BOOST_CHECK_EQUAL(clp->keys().size(), 2);

    auto data = clp->serialize();

    BOOST_CHECK_EQUAL(data["video_path"].asString(), "data/sample_video.webm");
    BOOST_CHECK_EQUAL(data["width"].asInt(), 80);
    BOOST_CHECK_EQUAL(data["height"].asInt(), 180);
    BOOST_CHECK(data.isMember("keys"));
    BOOST_CHECK_EQUAL(data["keys"][0]["frame"].asInt(), 120);
    BOOST_CHECK_EQUAL(data["keys"][0]["px"].asInt(), 40);
    BOOST_CHECK_EQUAL(data["keys"][0]["py"].asInt(), 60);
    BOOST_CHECK_EQUAL(data["keys"][0]["scale"].asFloat(), 1);
    BOOST_CHECK_EQUAL(data["keys"][0]["angle"].asInt(), 360000);
}

BOOST_AUTO_TEST_CASE(test_clipping_buffer_size) {
    BOOST_CHECK_EQUAL(clp->req_buffer_size(), 80 * 180 * 3);
}

BOOST_AUTO_TEST_CASE(test_clipping_get_keys) {
    auto k0 = clp->at(119);  // before first key frame
    auto k1 = clp->at(120);  // key
    auto k2 = clp->at(500);  // intermediate
    auto k3 = clp->at(1200); // key
    auto k4 = clp->at(1201); // after last key frame

    BOOST_CHECK_EQUAL(k0.computed(), true);
    BOOST_CHECK_EQUAL(k1.computed(), false);
    BOOST_CHECK_EQUAL(k2.computed(), true);
    BOOST_CHECK_EQUAL(k3.computed(), false);
    BOOST_CHECK_EQUAL(k4.computed(), true);

    BOOST_CHECK_EQUAL(k0.frame, 119);
    BOOST_CHECK_EQUAL(k0.angle(), k1.angle());
    BOOST_CHECK_EQUAL(k0.scale, k1.scale);
    BOOST_CHECK_EQUAL(k0.px, k1.px);
    BOOST_CHECK_EQUAL(k0.py, k1.py);

    BOOST_CHECK_EQUAL(k1.frame, 120);
    BOOST_CHECK_EQUAL(k1.angle(), 360);
    BOOST_CHECK_EQUAL(k1.scale, 1);
    BOOST_CHECK_EQUAL(k1.px, 40);
    BOOST_CHECK_EQUAL(k1.py, 60);

    BOOST_CHECK_EQUAL(k2.frame, 500);
    BOOST_CHECK_CLOSE(k2.angle(), 63.3333, 0.1);
    BOOST_CHECK_CLOSE(k2.scale, 0.82407, 0.1);
    BOOST_CHECK_EQUAL(k2.px, 54);
    BOOST_CHECK_EQUAL(k2.py, 81);

    BOOST_CHECK_EQUAL(k3.frame, 1200);
    BOOST_CHECK_EQUAL(k3.angle(), 180);
    BOOST_CHECK_CLOSE(k3.scale, 0.5, 0.00001);
    BOOST_CHECK_EQUAL(k3.px, 80);
    BOOST_CHECK_EQUAL(k3.py, 120);

    BOOST_CHECK_EQUAL(k4.frame, 1201);
    BOOST_CHECK_EQUAL(k4.angle(), k3.angle());
    BOOST_CHECK_EQUAL(k4.scale, k3.scale);
    BOOST_CHECK_EQUAL(k4.px, k3.px);
    BOOST_CHECK_EQUAL(k4.py, k3.py);
}

BOOST_AUTO_TEST_CASE(test_clipping_save_load) {
    const char *temp_path = "data/tmp/test_clipping_save.vcutter";
    std::remove(temp_path);

    clp->save(temp_path);

    BOOST_CHECK_EQUAL(clp->saved_path(), temp_path);

    vcutter::Clipping clp2(temp_path, false);

    BOOST_CHECK_EQUAL(clp2.saved_path(), temp_path);
    BOOST_CHECK_EQUAL(clp2.video_path(), "data/sample_video.webm");

    BOOST_CHECK_EQUAL(clp2.good(), true);
    BOOST_CHECK_EQUAL(clp2.w(), 80);
    BOOST_CHECK_EQUAL(clp2.h(), 180);
    BOOST_CHECK_EQUAL(clp2.keys().size(), 2);

    std::remove(temp_path);
}

BOOST_AUTO_TEST_CASE(test_clipping_wh) {
    clp->wh(90, 91);

    BOOST_CHECK_EQUAL(clp->w(), 90);
    BOOST_CHECK_EQUAL(clp->h(), 91);

    clp->w(80);
    clp->h(180);

    BOOST_CHECK_EQUAL(clp->w(), 80);
    BOOST_CHECK_EQUAL(clp->h(), 180);
}

BOOST_AUTO_TEST_CASE(test_clipping_remove) {
    ClippingDataStub clipping;

    vcutter::ClippingKey k1;
    k1.frame = 120;

    clipping.add(k1);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 1);
    clipping.remove(120);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 0);
}

BOOST_AUTO_TEST_CASE(test_clipping_define_start) {
    ClippingDataStub clipping;

    vcutter::ClippingKey k1;
    k1.frame = 120;

    for (int i = 0; i < 10; ++i) {
        clipping.add(k1);
        ++k1.frame;
    }

    BOOST_CHECK_EQUAL(clipping.keys().size(), 10);

    clipping.define_start(119);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 11);
    BOOST_CHECK_EQUAL(clipping.keys().begin()->frame, 119);

    clipping.define_start(120);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 10);
    BOOST_CHECK_EQUAL(clipping.keys().begin()->frame, 120);

    clipping.define_start(125);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 5);
    BOOST_CHECK_EQUAL(clipping.keys().begin()->frame, 125);

    clipping.define_start(140);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 1);
    BOOST_CHECK_EQUAL(clipping.keys().begin()->frame, 140);
}

BOOST_AUTO_TEST_CASE(test_clipping_define_end) {
    ClippingDataStub clipping;

    vcutter::ClippingKey k1;
    k1.frame = 120;

    for (int i = 0; i < 10; ++i) {
        clipping.add(k1);
        ++k1.frame;
    }

    BOOST_CHECK_EQUAL(clipping.keys().size(), 10);

    clipping.define_end(131);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 11);
    BOOST_CHECK_EQUAL(clipping.keys().rbegin()->frame, 131);

    clipping.define_end(129);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 10);
    BOOST_CHECK_EQUAL(clipping.keys().rbegin()->frame, 129);

    clipping.define_end(124);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 5);
    BOOST_CHECK_EQUAL(clipping.keys().rbegin()->frame, 124);

    clipping.define_end(100);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 1);
    BOOST_CHECK_EQUAL(clipping.keys().rbegin()->frame, 100);
}


BOOST_AUTO_TEST_CASE(test_clipping_remove_all) {
    ClippingDataStub clipping;

    vcutter::ClippingKey k1;
    k1.frame = 120;

    for (int i = 0; i < 10; ++i) {
        clipping.add(k1);
        ++k1.frame;
    }

    BOOST_CHECK_EQUAL(clipping.keys().size(), 10);

    clipping.remove_all(100);
    BOOST_CHECK_EQUAL(clipping.keys().size(), 1);
    BOOST_CHECK_EQUAL(clipping.keys().begin()->frame, 100);

    clipping.remove_all();
    BOOST_CHECK_EQUAL(clipping.keys().size(), 0);
}


BOOST_AUTO_TEST_CASE(test_clipping_first_last_frame) {
    ClippingDataStub clipping;

    vcutter::ClippingKey k1;
    k1.frame = 120;

    BOOST_CHECK(clipping.keys().empty());
    BOOST_CHECK_EQUAL(clipping.first_frame(), 1);
    BOOST_CHECK_EQUAL(clipping.last_frame(), 10000);

    for (int i = 0; i < 10; ++i) {
        clipping.add(k1);
        ++k1.frame;
    }

    BOOST_CHECK_EQUAL(clipping.first_frame(), 120);
    BOOST_CHECK_EQUAL(clipping.last_frame(), 129);
}

BOOST_AUTO_TEST_CASE(test_clipping_at_index) {
    ClippingDataStub clipping;

    vcutter::ClippingKey k1;
    k1.frame = 120;

    for (int i = 0; i < 10; ++i) {
        clipping.add(k1);
        ++k1.frame;
    }

    BOOST_CHECK_EQUAL(clipping.at_index(0).frame, 120);
    BOOST_CHECK_EQUAL(clipping.at_index(9).frame, 129);
}


BOOST_AUTO_TEST_CASE(test_clipping_find_index) {
    ClippingDataStub clipping;

    vcutter::ClippingKey k1;
    k1.frame = 120;

    for (int i = 0; i < 10; ++i) {
        clipping.add(k1);
        ++k1.frame;
    }

    BOOST_CHECK_EQUAL(clipping.find_index(100), -1);
    BOOST_CHECK_EQUAL(clipping.find_index(120), 0);
    BOOST_CHECK_EQUAL(clipping.find_index(125), 5);
}


BOOST_AUTO_TEST_CASE(test_clipping_version) {
    ClippingDataStub clipping;

    vcutter::ClippingKey k1;
    k1.frame = 120;

    BOOST_CHECK_EQUAL(clipping.version(), 0);


    for (int i = 0; i < 10; ++i) {
        clipping.add(k1);
        ++k1.frame;
    }

    BOOST_CHECK_EQUAL(clipping.version(), 10);

    clipping.remove(120);

    BOOST_CHECK_EQUAL(clipping.version(), 11);

    clipping.define_start(120);

    BOOST_CHECK_EQUAL(clipping.version(), 13);

    clipping.define_end(120);

    BOOST_CHECK_EQUAL(clipping.version(), 15);

    clipping.remove_all(100);

    BOOST_CHECK_EQUAL(clipping.version(), 17);

    clipping.remove_all();

    BOOST_CHECK_EQUAL(clipping.version(), 18);
}


BOOST_AUTO_TEST_CASE(test_clipping_video_path) {
    BOOST_CHECK_EQUAL(clp->video_path(), "data/sample_video.webm");
}

BOOST_AUTO_TEST_CASE(test_clipping_positionate_left) {
    ClippingTearDown teardown;

    vcutter::ClippingKey start_pos = clp->at(120);

    start_pos.px = clp->player()->info()->w() / 2;
    start_pos.py = clp->player()->info()->h() / 2;

    clp->add(start_pos);
    clp->positionate_left(120);

    vcutter::ClippingKey k1 = clp->at(120);
    BOOST_CHECK_EQUAL(k1.px, clp->w() / 2);
}

BOOST_AUTO_TEST_CASE(test_clipping_positionate_right) {
    ClippingTearDown teardown;

    vcutter::ClippingKey start_pos = clp->at(120);

    start_pos.px = clp->player()->info()->w() / 2;
    start_pos.py = clp->player()->info()->h() / 2;

    clp->add(start_pos);
    clp->positionate_right(120);

    vcutter::ClippingKey k1 = clp->at(120);

    BOOST_CHECK_EQUAL(k1.px, clp->player()->info()->w() - clp->w() / 2);
}


BOOST_AUTO_TEST_CASE(test_clipping_positionate_top) {
    ClippingTearDown teardown;

    vcutter::ClippingKey start_pos = clp->at(120);

    start_pos.px = clp->player()->info()->w() / 2;
    start_pos.py = clp->player()->info()->h() / 2;

    clp->add(start_pos);
    clp->positionate_top(120);
    vcutter::ClippingKey k1 = clp->at(120);
    BOOST_CHECK_EQUAL(k1.py, clp->h() / 2);
}


BOOST_AUTO_TEST_CASE(test_clipping_positionate_bottom) {
    ClippingTearDown teardown;

    vcutter::ClippingKey start_pos = clp->at(120);

    start_pos.px = clp->player()->info()->w() / 2;
    start_pos.py = clp->player()->info()->h() / 2;

    clp->add(start_pos);
    clp->positionate_bottom(120);
    vcutter::ClippingKey k1 = clp->at(120);
    BOOST_CHECK_EQUAL(k1.py, clp->player()->info()->h() - clp->h() / 2);
}


BOOST_AUTO_TEST_CASE(test_clipping_center_vertical) {
    ClippingTearDown teardown;

    vcutter::ClippingKey start_pos = clp->at(120);

    start_pos.px = clp->player()->info()->w() / 2;
    start_pos.py = clp->player()->info()->h() / 2;

    start_pos.py = 0;
    clp->add(start_pos);
    clp->center_vertical(120);
    vcutter::ClippingKey k1 = clp->at(120);
    BOOST_CHECK_EQUAL(k1.py, clp->player()->info()->h() / 2);
}

BOOST_AUTO_TEST_CASE(test_clipping_center_horizontal) {
    ClippingTearDown teardown;

    vcutter::ClippingKey start_pos = clp->at(120);

    start_pos.px = clp->player()->info()->w() / 2;
    start_pos.py = clp->player()->info()->h() / 2;

    start_pos.px = 0;
    clp->add(start_pos);
    clp->center_horizontal(120);
    vcutter::ClippingKey k1 = clp->at(120);
    BOOST_CHECK_EQUAL(k1.px, clp->player()->info()->w() / 2);
}


BOOST_AUTO_TEST_CASE(test_clipping_normalize_scale) {
    ClippingTearDown teardown;

    vcutter::ClippingKey start_pos = clp->at(120);

    start_pos.px = clp->player()->info()->w() / 2;
    start_pos.py = clp->player()->info()->h() / 2;
    start_pos.scale = 2;

    clp->add(start_pos);

    clp->wh(clp->player()->info()->w(), clp->player()->info()->h());

    clp->normalize_scale(120);
    vcutter::ClippingKey k1 = clp->at(120);

    BOOST_CHECK_CLOSE(k1.scale, 0.995073, 0.01);

    clp->wh(clp->player()->info()->w() * 2, clp->player()->info()->h() * 2);
    clp->normalize_scale(120);
    k1 = clp->at(120);

    BOOST_CHECK_CLOSE(k1.scale, 0.497536, 0.01);
}


/*

    functions to test:
    render
    render overloaded

    fit_left
    fit_right
    fit_top
    fit_bottom
    fit_all
*/

BOOST_AUTO_TEST_SUITE_END()