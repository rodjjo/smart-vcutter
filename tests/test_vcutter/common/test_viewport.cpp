#include "tests/testing.h"
#include "smart-vcutter/common/view_port.h"

BOOST_AUTO_TEST_SUITE(viewport_test_suite)

BOOST_AUTO_TEST_CASE(test_viewport_constructors) {
    int rawvp[] = {1, 2, 1280, 720};
    vcutter::viewport_t vp1;
    vcutter::viewport_t vp2(rawvp);
    vcutter::viewport_t vp3(1, 2, 3, 4);

    BOOST_CHECK_EQUAL(vp1[0], 0);
    BOOST_CHECK_EQUAL(vp1[1], 0);
    BOOST_CHECK_EQUAL(vp1[2], 1); // viewport must have at least one pixel
    BOOST_CHECK_EQUAL(vp1[3], 1); // viewport must have at least one pixel

    BOOST_CHECK_EQUAL(vp2[0], 1);
    BOOST_CHECK_EQUAL(vp2[1], 2);
    BOOST_CHECK_EQUAL(vp2[2], 1280);
    BOOST_CHECK_EQUAL(vp2[3], 720);

    BOOST_CHECK_EQUAL(vp3[0], 1);
    BOOST_CHECK_EQUAL(vp3[1], 2);
    BOOST_CHECK_EQUAL(vp3[2], 3);
    BOOST_CHECK_EQUAL(vp3[3], 4);
}

BOOST_AUTO_TEST_CASE(test_viewport_screen_to_frame_coords__point) {
    vcutter::viewport_t vp(0, 0, 640, 480);
    vcutter::point_t screen_center(320, 240);

    auto center_video = vp.screen_to_frame_coords(1280, 720, screen_center);
    BOOST_CHECK_EQUAL(center_video.x, 640);
    BOOST_CHECK_EQUAL(center_video.y, 360);
}

BOOST_AUTO_TEST_CASE(test_viewport_screen_to_frame_coords__box) {
    vcutter::viewport_t vp(0, 0, 856, 480);
    
    vcutter::point_t p[4] = {
        vcutter::point_t(214, 120), 
        vcutter::point_t(640, 120), 
        vcutter::point_t(640, 360), 
        vcutter::point_t(214, 360)
    };

    vcutter::box_t screen_box(p[0], p[1], p[2], p[3]);

    auto video_box = vp.screen_to_frame_coords(1280, 720, screen_box);

    BOOST_CHECK_CLOSE(video_box[0].x, 319, 0.1);
    BOOST_CHECK_CLOSE(video_box[0].y, 180, 0.1);
    BOOST_CHECK_CLOSE(video_box[1].x, 958, 0.1);
    BOOST_CHECK_CLOSE(video_box[1].y, 180, 0.1);
    BOOST_CHECK_CLOSE(video_box[2].x, 958, 0.1);
    BOOST_CHECK_CLOSE(video_box[2].y, 540, 0.1);
    BOOST_CHECK_CLOSE(video_box[3].x, 319, 0.1);
    BOOST_CHECK_CLOSE(video_box[3].y, 540, 0.1);
}

BOOST_AUTO_TEST_CASE(test_viewport_frame_to_screen_coords__point) {
    vcutter::viewport_t vp(0, 0, 640, 480);
    vcutter::point_t video_center(640, 360);

    auto screen_center = vp.frame_to_screen_coords(1280, 720, video_center);
    BOOST_CHECK_EQUAL(screen_center.x, 320);
    BOOST_CHECK_EQUAL(screen_center.y, 240);
}

BOOST_AUTO_TEST_CASE(test_viewport_frame_to_screen_coords__box) {
    vcutter::viewport_t vp(0, 0, 856, 480);
    
    vcutter::point_t p[4] = {
        vcutter::point_t(319, 180), 
        vcutter::point_t(958, 180), 
        vcutter::point_t(958, 540), 
        vcutter::point_t(319, 540)
    };

    vcutter::box_t video_box(p[0], p[1], p[2], p[3]);

    auto screen_box = vp.frame_to_screen_coords(1280, 720, video_box);

    BOOST_CHECK_CLOSE(screen_box[0].x, 214, 0.1);
    BOOST_CHECK_CLOSE(screen_box[0].y, 120, 0.1);
    BOOST_CHECK_CLOSE(screen_box[1].x, 640, 0.1);
    BOOST_CHECK_CLOSE(screen_box[1].y, 120, 0.1);
    BOOST_CHECK_CLOSE(screen_box[2].x, 640, 0.1);
    BOOST_CHECK_CLOSE(screen_box[2].y, 360, 0.1);
    BOOST_CHECK_CLOSE(screen_box[3].x, 214, 0.1);
    BOOST_CHECK_CLOSE(screen_box[3].y, 360, 0.1);
}

BOOST_AUTO_TEST_CASE(test_viewport_raster_zoom) {
    vcutter::viewport_t vp(0, 0, 640, 480);

    BOOST_CHECK_EQUAL(vp.raster_zoom(640, 480), 1);
    BOOST_CHECK_EQUAL(vp.raster_zoom(320, 480), 1);
    BOOST_CHECK_EQUAL(vp.raster_zoom(640, 240), 1);
    BOOST_CHECK_EQUAL(vp.raster_zoom(320, 240), 2);
    BOOST_CHECK_EQUAL(vp.raster_zoom(1280, 720), 0.5);
}

BOOST_AUTO_TEST_CASE(test_viewport_raster_coords) {
    vcutter::viewport_t vp(0, 0, 640, 480);

    vcutter::point_t coord[] = {
        vp.raster_coords(640, 480),
        vp.raster_coords(1280, 720),
        vp.raster_coords(720, 1280)
    };
    
    BOOST_CHECK_EQUAL(coord[0].x, 0);
    BOOST_CHECK_EQUAL(coord[0].y, 0);
    BOOST_CHECK_EQUAL(coord[1].x, 0);
    BOOST_CHECK_EQUAL(coord[1].y, 0.25);
    BOOST_CHECK_CLOSE(coord[2].x, 0.57812, 0.1);
    BOOST_CHECK_EQUAL(coord[2].y, 0);
        
}

BOOST_AUTO_TEST_CASE(test_viewport_fit) {
    vcutter::viewport_t vp(0, 0, 1280, 720);

    uint32_t w = 100, h = 90;
    BOOST_CHECK_EQUAL(vp.fit(&w, &h), 1.0);
    BOOST_CHECK_EQUAL(w, 100);
    BOOST_CHECK_EQUAL(h, 90);

    w = 1280 * 2;
    h = 90;
    BOOST_CHECK_EQUAL(vp.fit(&w, &h), 2);
    BOOST_CHECK_EQUAL(w, 1280);
    BOOST_CHECK_EQUAL(h, 45);

    w = 1280;
    h = 720 * 2;
    BOOST_CHECK_EQUAL(vp.fit(&w, &h), 2);
    BOOST_CHECK_EQUAL(w, 640);
    BOOST_CHECK_EQUAL(h, 720);

    w = 1290;
    h = 760;
    BOOST_CHECK_CLOSE(vp.fit(&w, &h), 1.055555, 0.1);
    BOOST_CHECK_EQUAL(w, 1222);
    BOOST_CHECK_EQUAL(h, 720);
}

BOOST_AUTO_TEST_SUITE_END()
