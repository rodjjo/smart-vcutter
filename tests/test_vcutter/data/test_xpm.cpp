#include <map>
#include <cstring>
#include "tests/testing.h"
#include "src/data/xpm.h"

namespace vcutter {
namespace xpm {

extern std::map<xpm_t, const char * const*> xpm_db;

}  // namespace vcutter
}  // namespace xpm


BOOST_AUTO_TEST_SUITE(xpm_test_suite)

BOOST_AUTO_TEST_CASE(text_xpm_image_sizes) {
    using namespace vcutter::xpm;
    BOOST_CHECK(strncmp(xpm_db[button_add][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_begin][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_compare][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_delete][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_edit][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_end][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_next][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_pause][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_play][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_prior][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_scissor][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_seek][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[button_stop][0], "24 24 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[cursor_dot][0], "32 32 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[cursor_drag][0], "32 32 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[cursor_resize][0], "32 32 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[cursor_rotate][0], "32 32 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[editor_apply][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[editor_apply_off][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[editor_resize][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[editor_rotate][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[editor_target1][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[editor_target2][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[arrow_dwn_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[boss_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[cd_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[clock_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[copy_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[directory_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[eject_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[erase_all_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[eye_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[expand_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[exit_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[film_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[green_pin_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[help_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[hint_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[left_right_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[lock_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[lupe_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[magic_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[move_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[note_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[paste_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[pencil_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[refresh_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[rotate_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[save_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[save_as_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[smile_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[take_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[tune_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[unlock_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[up_down_16x16][0], "16 16 ", 6) == 0);
    BOOST_CHECK(strncmp(xpm_db[yellow_pin_16x16][0], "16 16 ", 6) == 0);
}

BOOST_AUTO_TEST_SUITE_END()