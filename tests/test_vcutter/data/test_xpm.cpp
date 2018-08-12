#include <map>
#include "tests/testing.h"
#include "smart-vcutter/data/xpm.h"

BOOST_AUTO_TEST_SUITE(xpm_test_suite)

namespace {

bool check_image_size(vcutter::xpm::xpm_t id, int w, int h) {
    auto img = vcutter::xpm::image(id);
    return img->w() == w && img->h() == h;
}

}  // namespace

BOOST_AUTO_TEST_CASE(text_xpm_image_sizes) {
    using namespace vcutter::xpm;
    BOOST_CHECK(check_image_size(button_add, 24, 24));
    BOOST_CHECK(check_image_size(button_begin, 24, 24));
    BOOST_CHECK(check_image_size(button_compare, 24, 24));
    BOOST_CHECK(check_image_size(button_delete, 24, 24));
    BOOST_CHECK(check_image_size(button_edit, 24, 24));
    BOOST_CHECK(check_image_size(button_end, 24, 24));
    BOOST_CHECK(check_image_size(button_next, 24, 24));
    BOOST_CHECK(check_image_size(button_pause, 24, 24));
    BOOST_CHECK(check_image_size(button_play, 24, 24));
    BOOST_CHECK(check_image_size(button_prior, 24, 24));
    BOOST_CHECK(check_image_size(button_scissor, 24, 24));
    BOOST_CHECK(check_image_size(button_seek, 24, 24));
    BOOST_CHECK(check_image_size(button_stop, 24, 24));
    BOOST_CHECK(check_image_size(cursor_dot, 32, 32));
    BOOST_CHECK(check_image_size(cursor_drag, 32, 32));
    BOOST_CHECK(check_image_size(cursor_resize, 32, 32));
    BOOST_CHECK(check_image_size(cursor_rotate, 32, 32));
    BOOST_CHECK(check_image_size(editor_apply, 16, 16));
    BOOST_CHECK(check_image_size(editor_apply_off, 16, 16));
    BOOST_CHECK(check_image_size(editor_resize, 16, 16));
    BOOST_CHECK(check_image_size(editor_rotate, 16, 16));
    BOOST_CHECK(check_image_size(editor_target1, 16, 16));
    BOOST_CHECK(check_image_size(editor_target2, 16, 16));
    BOOST_CHECK(check_image_size(arrow_dwn_16x16, 16, 16));
    BOOST_CHECK(check_image_size(boss_16x16, 16, 16));
    BOOST_CHECK(check_image_size(cd_16x16, 16, 16));
    BOOST_CHECK(check_image_size(clock_16x16, 16, 16));
    BOOST_CHECK(check_image_size(copy_16x16, 16, 16));
    BOOST_CHECK(check_image_size(directory_16x16, 16, 16));
    BOOST_CHECK(check_image_size(eject_16x16, 16, 16));
    BOOST_CHECK(check_image_size(erase_all_16x16, 16, 16));
    BOOST_CHECK(check_image_size(eye_16x16, 16, 16));
    BOOST_CHECK(check_image_size(expand_16x16, 16, 16));
    BOOST_CHECK(check_image_size(exit_16x16, 16, 16));
    BOOST_CHECK(check_image_size(film_16x16, 16, 16));
    BOOST_CHECK(check_image_size(green_pin_16x16, 16, 16));
    BOOST_CHECK(check_image_size(help_16x16, 16, 16));
    BOOST_CHECK(check_image_size(hint_16x16, 16, 16));
    BOOST_CHECK(check_image_size(left_right_16x16, 16, 16));
    BOOST_CHECK(check_image_size(lock_16x16, 16, 16));
    BOOST_CHECK(check_image_size(lupe_16x16, 16, 16));
    BOOST_CHECK(check_image_size(magic_16x16, 16, 16));
    BOOST_CHECK(check_image_size(move_16x16, 16, 16));
    BOOST_CHECK(check_image_size(note_16x16, 16, 16));
    BOOST_CHECK(check_image_size(paste_16x16, 16, 16));
    BOOST_CHECK(check_image_size(pencil_16x16, 16, 16));
    BOOST_CHECK(check_image_size(refresh_16x16, 16, 16));
    BOOST_CHECK(check_image_size(rotate_16x16, 16, 16));
    BOOST_CHECK(check_image_size(save_16x16, 16, 16));
    BOOST_CHECK(check_image_size(save_as_16x16, 16, 16));
    BOOST_CHECK(check_image_size(smile_16x16, 16, 16));
    BOOST_CHECK(check_image_size(take_16x16, 16, 16));
    BOOST_CHECK(check_image_size(tune_16x16, 16, 16));
    BOOST_CHECK(check_image_size(unlock_16x16, 16, 16));
    BOOST_CHECK(check_image_size(up_down_16x16, 16, 16));
    BOOST_CHECK(check_image_size(yellow_pin_16x16, 16, 16));
}


BOOST_AUTO_TEST_SUITE_END()