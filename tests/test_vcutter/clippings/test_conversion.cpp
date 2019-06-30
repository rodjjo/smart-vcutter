/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <unistd.h>
#include "tests/testing.h"
#include "tests/test_vcutter/mocks/progress_handler.h"
#include "src/clippings/clipping.h"
#include "src/clippings/clipping_conversion.h"

namespace {

const char *kCONVERSION_PATH = "data/tmp/test_conversion_conversion.mp4";

std::shared_ptr<vcutter::Clipping> clp;

class TestConversionFixture {
 public:
    TestConversionFixture() {
        clp.reset(new vcutter::Clipping("data/sample_video.webm", true, vcutter::frame_callback_t()));
        BOOST_REQUIRE(clp->good());
        std::remove(kCONVERSION_PATH);
    }

    ~TestConversionFixture() {
        clp.reset();
        std::remove(kCONVERSION_PATH);
    }
};

}  // namespace

BOOST_AUTO_TEST_SUITE(clipping_conversion_tests)


BOOST_FIXTURE_TEST_CASE(test_convert_from_begin, TestConversionFixture) {
    std::shared_ptr<vcutter::ProgressHandler> prog(new ProgressHandlerMock());
    vcutter::ClippingConversion conversion(prog, clp);
    clp->wh(80, 82);

    BOOST_CHECK(conversion.convert("mp4-x264", kCONVERSION_PATH, 1000000, 24, true, false, 0));

    vcutter::Clipping clip(kCONVERSION_PATH, true, vcutter::frame_callback_t());

    BOOST_CHECK_EQUAL(clip.w(), 80u);
    BOOST_CHECK_EQUAL(clip.h(), 82u);
    BOOST_CHECK_EQUAL(clip.last_frame(), clp->last_frame());
}

BOOST_FIXTURE_TEST_CASE(test_convert_from_end, TestConversionFixture) {
    std::shared_ptr<vcutter::ProgressHandler> prog(new ProgressHandlerMock());
    vcutter::ClippingConversion conversion(prog, clp);
    clp->wh(80, 82);

    BOOST_CHECK(conversion.convert("mp4-x264", kCONVERSION_PATH, 1000000, 24, false, false, 0));

    vcutter::Clipping clip(kCONVERSION_PATH, true, vcutter::frame_callback_t());

    BOOST_CHECK_EQUAL(clip.w(), 80u);
    BOOST_CHECK_EQUAL(clip.h(), 82u);
    BOOST_CHECK_EQUAL(clip.last_frame(), clp->last_frame());
}

BOOST_FIXTURE_TEST_CASE(test_convert_append, TestConversionFixture) {
    std::shared_ptr<vcutter::ProgressHandler> prog(new ProgressHandlerMock());
    vcutter::ClippingConversion conversion(prog, clp);
    clp->wh(80, 82);

    BOOST_CHECK(conversion.convert("mp4-x264", kCONVERSION_PATH, 1000000, 24, true, true, 0));

    vcutter::Clipping clip(kCONVERSION_PATH, true, vcutter::frame_callback_t());

    BOOST_CHECK_EQUAL(clip.w(), 80u);
    BOOST_CHECK_EQUAL(clip.h(), 82u);
    BOOST_CHECK_EQUAL(clip.last_frame(), clp->last_frame() * 2 - 2);
}


BOOST_FIXTURE_TEST_CASE(test_convert_transition_frames, TestConversionFixture) {
    std::shared_ptr<vcutter::ProgressHandler> prog(new ProgressHandlerMock());
    clp->wh(80, 82);
    clp->add(clp->at(1));
    clp->add(clp->at(20));

    vcutter::ClippingConversion conversion(prog, clp, clp->w() * clp->h() * 15);

    BOOST_CHECK(conversion.convert("mp4-x264", kCONVERSION_PATH, 1000000, 24, false, false, 5));

    vcutter::Clipping clip(kCONVERSION_PATH, true, vcutter::frame_callback_t());

    BOOST_CHECK_EQUAL(clip.w(), 80u);
    BOOST_CHECK_EQUAL(clip.h(), 82u);
    BOOST_CHECK_EQUAL(clip.last_frame(), 15u);
}

BOOST_AUTO_TEST_SUITE_END()

