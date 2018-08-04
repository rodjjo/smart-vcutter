#ifndef SMART_VCUTTER_DATA_XPM_H
#define SMART_VCUTTER_DATA_XPM_H

#include <memory>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Pixmap.H>


namespace vcutter {
namespace xpm {

typedef enum {
    no_image,
    cursor_dot,
    cursor_rotate,
    cursor_resize,
    cursor_drag,
    button_begin,
    button_end,
    button_next,
    button_pause,
    button_play,
    button_prior,
    button_scissor,
    button_stop,
    button_seek,
    button_add,
    button_compare,
    button_delete,
    button_edit,
    editor_rotate,
    editor_resize,
    editor_target1,
    editor_target2,
    editor_apply,
    editor_apply_off,
    directory_16x16,
    eject_16x16,
    exit_16x16,
    film_16x16,
    green_pin_16x16,
    save_16x16,
    save_as_16x16,
    take_16x16,
    yellow_pin_16x16,
    arrow_dwn_16x16,
    boss_16x16,
    cd_16x16,
    clock_16x16,
    copy_16x16,
    erase_all_16x16,
    eye_16x16,
    help_16x16,
    refresh_16x16,
    hint_16x16,
    lock_16x16,
    lupe_16x16,
    magic_16x16,
    note_16x16,
    paste_16x16,
    pencil_16x16,
    smile_16x16,
    tune_16x16,
    unlock_16x16,
    expand_16x16,
    left_right_16x16,
    move_16x16,
    rotate_16x16,
    up_down_16x16
} xpm_t;

const char *const *data(xpm_t xpm_id);
std::shared_ptr<Fl_Pixmap> pixmap(xpm_t xpm_id);
std::shared_ptr<Fl_Pixmap> pixmap(const char *const *xpm_data);
std::shared_ptr<Fl_RGB_Image> image(xpm_t xpm_id,  Fl_Color bg=FL_GRAY);
std::shared_ptr<Fl_RGB_Image> image(const char *const *xpm_data,  Fl_Color bg=FL_GRAY);

} // namespace xpm
} // namespace vcutter

#endif
