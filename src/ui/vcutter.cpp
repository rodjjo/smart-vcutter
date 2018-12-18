/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/ui/vcutter.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/vstream/video_stream.h"
#include "src/common/utils.h"


#include <FL/Fl.H>

const int kMENU_HEIGHT = 25;


namespace vcutter {

int default_window_width() {
  int value = 1280;
  if (value + 50 > Fl::w()) {
    value = Fl::w() - 50;
  }
  return value;
}

int default_window_height() {
  int value = 720;
  if (value + 100 > Fl::h()) {
    value = Fl::h() - 100;
  }
  return value;
}

int default_window_left() {
  return (Fl::w() - default_window_width()) / 2;
}

int default_window_top() {
  return (Fl::h() - default_window_height()) / 2;
}

VCutter::VCutter() : Fl_Menu_Window(
    default_window_left(),
    default_window_top(),
    default_window_width(),
    default_window_height()
) {
    this->size_range(default_window_width(), default_window_height());

    init_actions();

    init_controls();

    init_menu();

    for (auto c : controls_) {
        c->save_reference(w(), h());
    }

    this->show();

    auto_size();

    clipping_version_ = -2;
}


VCutter::~VCutter() {
}

void VCutter::init_menu() {
    this->begin();
    menu_ = new MenuBar2(default_window_width(), kMENU_HEIGHT);
    this->end();

    menu_->add("&File/&Open video[^o]", open_action_, xpm::film_16x16);
}

void VCutter::init_controls() {

}

void VCutter::init_actions() {
    clipping_.reset(new ClippingActions2(this));
    open_action_.reset(new Action([this] () { open_file(); }));
}

void VCutter::auto_size() {
    menu_->size(w(), kMENU_HEIGHT);
    for (auto c : controls_) {
        c->auto_size(w(), h());
    }
}

bool VCutter::close_file() {
    if (clipping_->clipping() && clipping_->clipping()->version() != clipping_version_) {
        int opt = yes_nc("Do you want to save the modifications?");
        if (opt == 0) {
            if (!clipping_->save()) {
                return false;
            }
        } else if (opt == 1) {
            return false;
        }
    }

    clipping_->close();
    return true;
}

void VCutter::open_file(const char *path) {
    if (!close_file()) {
        return;
    }

    clipping_->open(path);
}

void VCutter::handle_clipping_opened(bool opened)  {
    if (opened) {
        clipping_version_ = clipping_->clipping()->version();
    }

}

void VCutter::handle_clipping_resized() {

}

void VCutter::handle_clipping_keys_changed() {

}

void VCutter::handle_buffer_modified()  {

}

void VCutter::run(const char *filepath) {
    if (filepath && clipping_->clipping() != NULL) {
        open_file(filepath);
    }

    if (Fl::run()) {
        throw std::string("Fl::run error");
    }
}

}  // namespace vcutter

#ifndef UNIT_TEST

int main(int argc, char **argv) {
    vs::initialize();
    Fl::scheme("gtk+");

    vcutter::VCutter *main_window = new vcutter::VCutter();
    main_window->run(argc > 0 ? argv[1] : NULL);

    return 0;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
     return main(__argc, __argv);
}
#endif

#endif  // UNIT_TEST
