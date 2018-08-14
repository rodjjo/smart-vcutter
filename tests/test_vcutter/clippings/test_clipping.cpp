#include <cstdio>
#include <memory>

#include "tests/testing.h"
#include "src/clippings/clipping.h"

namespace {

std::unique_ptr<vcutter::Clipping> clp;

class SuiteFixture {
 public:
    SuiteFixture() {
        clp.reset(new vcutter::Clipping("data/sample_video.webm", true));
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

    ~SuiteFixture() {
        clp.reset();
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

    vcutter::Clipping clp2(temp_path, false);

    BOOST_CHECK_EQUAL(clp2.good(), true);
    BOOST_CHECK_EQUAL(clp2.w(), 80);
    BOOST_CHECK_EQUAL(clp2.h(), 180);
    BOOST_CHECK_EQUAL(clp2.keys().size(), 2);

    std::remove(temp_path);
}

/*
    functions to test:
    render
    render overloaded
    remove
    remove_before
    remove_after
    remove_others
    remove_all
    first_frame
    last_frame
    wh
    at_index
    find_index
    video_path
    version
    positionate_left
    positionate_right
    positionate_top
    positionate_bottom
    positionate_vertical
    positionate_horizontal
    normalize_scale
    align_left
    align_right
    align_top
    align_bottom
    align_all
*/

BOOST_AUTO_TEST_SUITE_END()