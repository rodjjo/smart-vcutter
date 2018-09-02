/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_CUTTER_CUTTER_WINDOW_H_
#define SRC_WND_CUTTER_CUTTER_WINDOW_H_

#include <inttypes.h>
#include <string>
#include <memory>
#include <FL/Fl.H>
#include <FL/Fl_Group.H>

#include "src/wnd_cutter/clipping_actions.h"
#include "src/wnd_cutter/player_bar.h"
#include "src/wnd_cutter/side_bar.h"
#include "src/viewer/editor/clipping_editor.h"


namespace vcutter {

class CutterWindow : public ClippingActionsHandler {
 public:
    CutterWindow(Fl_Group *parent);
    virtual ~CutterWindow();

    ClippingActions *clipping_actions();
    Clipping *clipping();
    PlayerWrapper *player();

    void close();
    std::shared_ptr<ClippingRender> to_clipping();
    void poll_actions();
    bool visible();
    bool modified();
    uint64_t modified_version();
    void clear_modified();
    void cancel_operations();

    std::string get_video_path();
    void action_goto_reference();
    void action_create_ref();
    void action_use_ref(bool positionate_x, bool positionate_y, bool rotate, bool scale);
    void action_clear_ref();
    void action_toggle_compare();
    void action_toggle_compare_wink();

    bool compare_enabled();
    bool compare_alternate();

    void resize_controls();
 private:
    void handle_clipping_resized() override;
    void handle_clipping_opened(bool opened) override;
    void handle_clipping_keys_changed() override;
    void handle_buffer_modified() override;
    bool clipping_actions_active() override;

 private:
    void clear(bool clear_controls = true);
    void open_video();
    void frame_sleep(double fps);
    void redraw_frame(bool update_key_list=false);
    void set_widget_image(Fl_Widget* widget, std::shared_ptr<Fl_Image> image);
    void update_title();

 private:
    void update_buffers(bool frame_changed);
    void double_click(void *component);

 private:
    Fl_Group *parent_;
    Fl_Group *window_;
    Fl_Group *components_group_;
    ClippingEditor *clipping_editor_;

    std::unique_ptr<ClippingActions> clipping_actions_;
    std::unique_ptr<PlayerBar> player_bar_;
    std::unique_ptr<SideBar> side_bar_;

  private:
    uint64_t clipping_version_;
    unsigned int wink_lap_;
    bool wink_comparison_;
    bool open_failure_;
    bool in_key_list_;
};

}  // namespace vcutter

#endif  // SRC_WND_CUTTER_CUTTER_WINDOW_H_
