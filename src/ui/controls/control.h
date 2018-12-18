/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_UI_CONTROLS_CONTROL_H_
#define SRC_UI_CONTROLS_CONTROL_H_

#include <list>

namespace vcutter {

class Control {
 public:
    Control();
    virtual ~Control();
    virtual int w() = 0;
    virtual int h() = 0;
    virtual int x() = 0;
    virtual int y() = 0;
    virtual void coordinates(int x, int y, int w, int h) = 0;
    void save_reference(int window_w, int window_h);
    void auto_size(int w, int h);
    int rx();
    int ry();
    int rw();
    int rh();

 public:
    int window_w_;
    int window_h_;
    int rx_;
    int ry_;
    int rw_;
    int rh_;
};

}  // namespace vcutter

#endif  // SRC_UI_CONTROLS_CONTROL_H_
