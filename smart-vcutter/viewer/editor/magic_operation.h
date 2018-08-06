/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_VIEWER_EDITOR_MAGIC_OPERATION_H_
#define SMART_VCUTTER_VIEWER_EDITOR_MAGIC_OPERATION_H_

#include "smart-vcutter/viewer/editor/clipping_operations.h"
#include "smart-vcutter/viewer/viewer_texture.h"

namespace vcutter {

class MagicOperation: public ClippingOperation {
 public:
    explicit MagicOperation(const char* name, bool use_reference);
    virtual ~MagicOperation();
    void activate() override;
    bool active() override;
    void draw() override;
    clipping_key_t get_transformed_key() override;
    void cancel() override;
    bool should_redraw() override;
    void mouse_changed(char direction) override;
    Fl_RGB_Image *current_cursor() override;
    void set_option(const char *opt_name, int value) override;
    int get_option(const char *opt_name) override;
 private:
    void to_gl_coords(float *x, float *y);
    void set_near_points(bool value);
    void set_near_apply(bool value);
    bool near_point_1();
    bool near_point_2();
    bool near_apply();
    void apply();
 private:
    uint32_t px1_;
    uint32_t py1_;
    uint32_t px2_;
    uint32_t py2_;
    int frame_;
    bool opt_x_enabled_;
    bool opt_y_enabled_;
    bool opt_scale_enabled_;
    bool opt_rotate_enabled_;
    bool drag_point1_;
    bool drag_point2_;
    bool near_points_;
    bool near_apply_;
    bool points_defined_;
    bool preview_line_;
    bool active_;
    bool use_reference_;
    bool should_redraw_;
    std::shared_ptr<Fl_RGB_Image> cursor_;
    std::shared_ptr<ViewerTexture> target1_;
    std::shared_ptr<ViewerTexture> target2_;
    std::shared_ptr<ViewerTexture> apply_;
    std::shared_ptr<ViewerTexture> apply_off_;
};

}  // namespace vcutter

#endif  // SMART_VCUTTER_VIEWER_EDITOR_MAGIC_OPERATION_H_
