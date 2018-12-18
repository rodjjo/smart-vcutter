/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_UI_VCUTTER_H_
#define SRC_UI_VCUTTER_H_

#include <memory>
#include <FL/Fl_Menu_Window.H>
#include "src/ui/controls/control.h"
#include "src/ui/controls/menu_bar.h"
#include "src/ui/clipping_actions.h"


namespace vcutter {

typedef std::list<Control *> control_list_t;

class VCutter: public Fl_Menu_Window, public ClippingActions2Handler {
 public:
    VCutter();
    virtual ~VCutter();
    void run(const char *filepath);

    void handle_clipping_opened(bool opened) override;
    void handle_clipping_resized() override;
    void handle_clipping_keys_changed() override;
    void handle_buffer_modified() override;

 private:
    void init_controls();
    void init_menu();
    void init_actions();
    void auto_size();

 private:
    bool close_file();
    void save_file();
    void open_file(const char *path = NULL);

 private:
    control_list_t controls_;
    MenuBar2 *menu_;
    uint64_t clipping_version_;
    std::shared_ptr<Action> open_action_;
    std::unique_ptr<ClippingActions2> clipping_;
};



}  // namespace vcutter

#endif  // SRC_UI_VCUTTER_H_
