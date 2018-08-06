/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_VIEWER_CLIPPING_OPERATIONS_H_
#define SMART_VCUTTER_VIEWER_CLIPPING_OPERATIONS_H_

#include <list>
#include <memory>
#include <string>

#include <FL/Fl_RGB_Image.H>
#include "smart-vcutter/data/xpm.h"


#include "smart-vcutter/wrappers/video_player.h"
#include "smart-vcutter/clippings/clipping_keeper.h"

namespace vcutter {


box_t current_clipping_box(const viewport_t &vp, PlayerWrapper *player, ClippingKeeper *keeper, bool *computed=NULL);

class ClippingOperationSet;

class ClippingOperation {
 public:
    explicit ClippingOperation(const char *name);
    virtual ~ClippingOperation(){}
    virtual void activate() {} // turns active by its menu
    const char *name();
    virtual bool active() = 0;
    virtual void draw();
    virtual void draw_dragging_points();
    virtual clipping_key_t get_transformed_key() = 0;
    virtual void cancel() = 0;
    virtual bool should_redraw() = 0;
    virtual void mouse_changed(char direction) = 0;
    virtual Fl_RGB_Image *current_cursor() = 0;
    virtual void set_option(const char *opt_name, int value);
    virtual int get_option(const char *opt_name);
    virtual void alt_pressed();
 protected:
     PlayerWrapper *player();
     ClippingKeeper *keeper();
     void add_key();
     void modify();
     int mouse_down_x();
     int mouse_down_y();
     int mouse_move_x();
     int mouse_move_y();
     const viewport_t & view_port();
 private:
    friend class ClippingOperationSet;
    void register_operation(PlayerWrapper **player_var, ClippingKeeper **keeper_var, bool *modified_var);
    void mouse_down(bool left_pressed, bool right_pressed, int x, int y);
    void mouse_move(bool left_pressed, bool right_pressed, int dx, int dy, int mx, int my);
    void mouse_up(bool left_pressed, bool right_pressed, int dx, int dy, int ux, int uy);
    void view_port(const viewport_t &vp);
    void _cancel();
 private:
    std::string name_;
    PlayerWrapper **player_;
    ClippingKeeper **keeper_;
    viewport_t view_port_;
    bool *modified_;
    int mouse_move_x_;
    int mouse_move_y_;
    int mouse_down_x_;
    int mouse_down_y_;
};

class ClippingOperationSet {
 public:
    ClippingOperationSet(PlayerWrapper **player_var, ClippingKeeper **keeper_var);
    virtual ~ClippingOperationSet();
    void register_operation(std::shared_ptr<ClippingOperation> operation);
    void activate(const char *name);
    void draw(const viewport_t &vp);
    void draw_ref_line(const viewport_t &vp);
    void draw_dragging_points(const viewport_t &vp);
    bool should_redraw(const viewport_t &vp);
    bool modified(bool clear_flag);
    void mouse_down(const viewport_t &vp, bool left_pressed, bool right_pressed, int x, int y);
    void mouse_move(const viewport_t &vp, bool left_pressed, bool right_pressed, int dx, int dy, int mx, int my);
    void mouse_up(const viewport_t &vp, bool left_pressed, bool right_pressed, int dx, int dy, int ux, int uy);
    void cancel();
    bool is_active(const char *operation);
    clipping_key_t get_transformed_key();
    void alt_pressed();
    void set_option(const char *operation_name, const char *option_name, int value);
    int get_option(const char *operation_name, const char *option_name);
    Fl_RGB_Image *current_cursor(const viewport_t &vp);
 private:
    void draw_current_box(const viewport_t &vp);
    void draw_next_box(const viewport_t &vp);
    void draw_box(const viewport_t &vp, box_t box, bool invert, float r, float g, float b, float a);

    bool has_active_operation();
 private:
    std::list<std::shared_ptr<ClippingOperation> > operations_;
    PlayerWrapper **player_;
    ClippingKeeper **keeper_;
    bool modified_;
};

}  // namespace vcutter


#endif  // SMART_VCUTTER_VIEWER_CLIPPING_OPERATIONS_H_