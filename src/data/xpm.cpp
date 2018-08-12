/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <map>
#include "src/data/xpm.h"

#pragma GCC diagnostic push
// there is no problem, i dont use it as a char *, i do as const char*
// i'd rather not modify .xpm files because it's generated by gimp and i keep it as it is.
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "src/data/xpm/cursor_rotate.xpm"
#include "src/data/xpm/cursor_resize.xpm"
#include "src/data/xpm/cursor_dot.xpm"
#include "src/data/xpm/cursor_drag.xpm"
#include "src/data/xpm/button_begin.xpm"
#include "src/data/xpm/button_end.xpm"
#include "src/data/xpm/button_next.xpm"
#include "src/data/xpm/button_pause.xpm"
#include "src/data/xpm/button_play.xpm"
#include "src/data/xpm/button_prior.xpm"
#include "src/data/xpm/button_scissor.xpm"
#include "src/data/xpm/button_stop.xpm"
#include "src/data/xpm/button_seek.xpm"
#include "src/data/xpm/button_add.xpm"
#include "src/data/xpm/button_compare.xpm"
#include "src/data/xpm/button_delete.xpm"
#include "src/data/xpm/button_edit.xpm"
#include "src/data/xpm/editor_rotate.xpm"
#include "src/data/xpm/editor_resize.xpm"
#include "src/data/xpm/editor_target1.xpm"
#include "src/data/xpm/editor_target2.xpm"
#include "src/data/xpm/editor_apply.xpm"
#include "src/data/xpm/editor_apply_off.xpm"
#include "src/data/xpm/menu/directory_16x16.xpm"
#include "src/data/xpm/menu/eject_16x16.xpm"
#include "src/data/xpm/menu/exit_16x16.xpm"
#include "src/data/xpm/menu/film_16x16.xpm"
#include "src/data/xpm/menu/green_pin_16x16.xpm"
#include "src/data/xpm/menu/save_16x16.xpm"
#include "src/data/xpm/menu/save_as_16x16.xpm"
#include "src/data/xpm/menu/take_16x16.xpm"
#include "src/data/xpm/menu/yellow_pin_16x16.xpm"
#include "src/data/xpm/menu/arrow_dwn_16x16.xpm"
#include "src/data/xpm/menu/boss_16x16.xpm"
#include "src/data/xpm/menu/cd_16x16.xpm"
#include "src/data/xpm/menu/clock_16x16.xpm"
#include "src/data/xpm/menu/copy_16x16.xpm"
#include "src/data/xpm/menu/erase_all_16x16.xpm"
#include "src/data/xpm/menu/eye_16x16.xpm"
#include "src/data/xpm/menu/help_16x16.xpm"
#include "src/data/xpm/menu/refresh_16x16.xpm"
#include "src/data/xpm/menu/hint_16x16.xpm"
#include "src/data/xpm/menu/lock_16x16.xpm"
#include "src/data/xpm/menu/lupe_16x16.xpm"
#include "src/data/xpm/menu/magic_16x16.xpm"
#include "src/data/xpm/menu/note_16x16.xpm"
#include "src/data/xpm/menu/paste_16x16.xpm"
#include "src/data/xpm/menu/pencil_16x16.xpm"
#include "src/data/xpm/menu/smile_16x16.xpm"
#include "src/data/xpm/menu/tune_16x16.xpm"
#include "src/data/xpm/menu/unlock_16x16.xpm"
#include "src/data/xpm/menu/expand_16x16.xpm"
#include "src/data/xpm/menu/left_right_16x16.xpm"
#include "src/data/xpm/menu/move_16x16.xpm"
#include "src/data/xpm/menu/rotate_16x16.xpm"
#include "src/data/xpm/menu/up_down_16x16.xpm"

#pragma GCC diagnostic pop

namespace vcutter {
namespace xpm {

std::map<xpm_t, const char * const*> xpm_db = {
    { cursor_dot, cursor_dot_xpm },
    { cursor_rotate, cursor_rotate_xpm },
    { cursor_resize, cursor_resize_xpm },
    { cursor_drag, cursor_drag_xpm },
    { button_begin, button_begin_xpm },
    { button_end, button_end_xpm },
    { button_next, button_next_xpm },
    { button_pause, button_pause_xpm },
    { button_play, button_play_xpm },
    { button_prior, button_prior_xpm },
    { button_scissor, button_scissor_xpm },
    { button_stop, button_stop_xpm },
    { button_seek, button_seek_xpm },
    { button_add, button_add_xpm },
    { button_compare, button_compare_xpm },
    { button_delete, button_delete_xpm },
    { button_edit, button_edit_xpm },
    { editor_rotate, editor_rotate_xpm },
    { editor_resize, editor_resize_xpm },
    { editor_target1, editor_target1_xpm },
    { editor_target2, editor_target2_xpm },
    { editor_apply, editor_apply_xpm },
    { editor_apply_off, editor_apply_off_xpm },
    { directory_16x16, directory_16x16_xpm },
    { eject_16x16, eject_16x16_xpm },
    { exit_16x16, exit_16x16_xpm },
    { film_16x16, film_16x16_xpm },
    { green_pin_16x16, green_pin_16x16_xpm },
    { save_16x16, save_16x16_xpm },
    { save_as_16x16, save_as_16x16_xpm },
    { take_16x16, take_16x16_xpm },
    { yellow_pin_16x16, yellow_pin_16x16_xpm },
    { yellow_pin_16x16, yellow_pin_16x16_xpm },
    { arrow_dwn_16x16, arrow_dwn_16x16_xpm },
    { boss_16x16, boss_16x16_xpm },
    { cd_16x16, cd_16x16_xpm },
    { clock_16x16, clock_16x16_xpm },
    { copy_16x16, copy_16x16_xpm },
    { erase_all_16x16, erase_all_16x16_xpm },
    { eye_16x16, eye_16x16_xpm },
    { help_16x16, help_16x16_xpm },
    { refresh_16x16, refresh_16x16_xpm },
    { hint_16x16, hint_16x16_xpm },
    { lock_16x16, lock_16x16_xpm },
    { lupe_16x16, lupe_16x16_xpm },
    { magic_16x16, magic_16x16_xpm },
    { note_16x16, note_16x16_xpm },
    { paste_16x16, paste_16x16_xpm },
    { pencil_16x16, pencil_16x16_xpm },
    { smile_16x16, smile_16x16_xpm },
    { tune_16x16, tune_16x16_xpm },
    { unlock_16x16, unlock_16x16_xpm },
    { expand_16x16, expand_16x16_xpm },
    { left_right_16x16, left_right_16x16_xpm },
    { move_16x16, move_16x16_xpm },
    { rotate_16x16, rotate_16x16_xpm },
    { up_down_16x16, up_down_16x16_xpm }
};

std::shared_ptr<Fl_RGB_Image> image(xpm_t xpm_id, Fl_Color bg) {
    Fl_Pixmap image( xpm_db[xpm_id]);
    return std::shared_ptr<Fl_RGB_Image>(new Fl_RGB_Image(&image, bg));
}

} // namespace xpm
}  // namespace vcutter
