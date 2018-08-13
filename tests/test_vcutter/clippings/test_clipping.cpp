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
    BOOST_CHECK_EQUAL(clp->keys().size(), 1);

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


BOOST_AUTO_TEST_SUITE_END()