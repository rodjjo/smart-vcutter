/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <GL/gl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <FL/Fl.H>
#include <FL/gl.h>

#include "src/viewer/buffer_viewer.h"

namespace vcutter {

BufferViewer::BufferViewer(uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Fl_Gl_Window(x, y, w, h) {
    init(NULL, NULL);
}

BufferViewer::BufferViewer(BufferSupplier *supplier, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Fl_Gl_Window(x, y, w, h) {
    init(supplier, NULL);
}

BufferViewer::BufferViewer(DrawHandler *observer, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Fl_Gl_Window(x, y, w, h) {
    init(NULL, observer);
}

BufferViewer::BufferViewer(BufferSupplier *supplier, DrawHandler *observer, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Fl_Gl_Window(x, y, w, h) {
    init(supplier, observer);
}

void BufferViewer::init(BufferSupplier *supplier, DrawHandler *observer) {
    supplier_ = supplier;
    observer_ = observer;
    mouse_down_left_ = false;
    mouse_down_right_ = false;
    mouse_down_x_ = 0;
    mouse_down_y_ = 0;
    buffer_usage_ = 0;
    buffer_size_ = 0;
    valid(0);
    vp_[0] = 0;
    vp_[1] = 0;
    vp_[2] = this->w();
    vp_[3] = this->h();
}

BufferViewer::~BufferViewer() {
}

void BufferViewer::cancel_operations() {
    if (observer_) {
        mouse_down_left_ = false;
        mouse_down_right_ = false;
        observer_->viewer_mouse_cancel(this); // cancels previous operation
        this->redraw();
    }
}

int BufferViewer::handle(int event) {
    if (!observer_) {
        return Fl_Gl_Window::handle(event);
    }

    switch (event) {
        case FL_KEYUP:
        case FL_KEYDOWN: {
            if (Fl::event_key() == FL_Alt_L) {
                if (event == FL_KEYUP) {
                    mouse_down_x_ = Fl::event_x();
                    mouse_down_y_ = Fl::event_y();
                    observer_->viewer_mouse_move(this, mouse_down_left_, mouse_down_right_, mouse_down_x_, mouse_down_y_, Fl::event_x(), Fl::event_y());
                    observer_->viewer_mouse_alt(this);
                }
            } else {
                return Fl_Gl_Window::handle(event);
            }
        }
        break;

        case FL_MOVE:
        case FL_DRAG: {
            if (!mouse_down_left_ && !mouse_down_right_) {
                mouse_down_x_ = Fl::event_x();
                mouse_down_y_ = Fl::event_y();
            }
            observer_->viewer_mouse_move(this, mouse_down_left_, mouse_down_right_, mouse_down_x_, mouse_down_y_, Fl::event_x(), Fl::event_y());
        } break;

        case FL_PUSH: {
            bool mouse_down_left = Fl::event_button() == FL_LEFT_MOUSE;
            bool mouse_down_right = !mouse_down_left && Fl::event_button() == FL_RIGHT_MOUSE;

            if (mouse_down_left || mouse_down_right) {
                mouse_down_x_ = Fl::event_x();
                mouse_down_y_ = Fl::event_y();
                mouse_down_left_ = mouse_down_left;
                mouse_down_right_ = mouse_down_right;
                observer_->viewer_mouse_down(this, mouse_down_left_, mouse_down_right_, mouse_down_x_, mouse_down_y_);
            } else {
                return Fl_Gl_Window::handle(event);
            }
        } break;

        case FL_RELEASE: {
            if (Fl::event_button() != FL_LEFT_MOUSE && Fl::event_button() != FL_RIGHT_MOUSE) {
                return Fl_Gl_Window::handle(event);
            }
            if (mouse_down_left_ || mouse_down_right_) {
                observer_->viewer_mouse_up(this, mouse_down_left_, mouse_down_right_,  mouse_down_x_, mouse_down_y_, Fl::event_x(), Fl::event_y());
                mouse_down_left_ = false;
                mouse_down_right_ = false;
            }
        } break;

        default:
            return Fl_Gl_Window::handle(event);
    }

    return 1;
}

void BufferViewer::draw() {
    unsigned int w = 0, h = 0;
    const unsigned char *buffer = NULL;

    if (!valid()) {
        valid(1);
        glLoadIdentity();
        glViewport(0, 0, this->w(), this->h());
    }

    vp_.update();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (supplier_) {
        supplier_->viewer_buffer(this, &buffer, &w, &h);
    }

    if (observer_) {
        bool handled = false;
        observer_->viewer_draw(this, &handled, buffer, w, h);
        if (handled) {
            return;
        }
    }

    if (buffer != NULL && w != 0 && h != 0) {
        draw_buffer(buffer, w, h);
    }

    if (observer_) {
        observer_->viewer_after_draw(this);
    }
}

void BufferViewer::draw_overlay() {
    // do nothing...
}


const viewport_t & BufferViewer::view_port() const {
    return vp_;
}

void BufferViewer::draw_buffer(const unsigned char* buffer, uint32_t w, uint32_t h) {
    update_cache(&buffer, &w, &h);

    float pixel_zoom = vp_.raster_zoom(w, h);
    point_t raster = vp_.raster_coords(w, h);

    glRasterPos2f(-1.0 + raster.x, 1.0 - raster.y);
    glPixelZoom(pixel_zoom, -pixel_zoom);
    if (w % 4 == 0)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    else
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);

    glRasterPos2f(0.0f, 0.0f);
    glPixelZoom(1.0f, 1.0f);
}

void BufferViewer::update_cache(const unsigned char** buffer, uint32_t *w, uint32_t *h) {
    unsigned int required_size = (*w) * (*h) * 3;
    unsigned int nw = *w, nh = *h;

    view_port().fit(&nw, &nh);

    unsigned int vp_size = nw * nh * 3;

    if (required_size > vp_size * 0.6) {
        buffer_.reset(); // do not keep cache data
        buffer_size_ = 0;
        return;
    }

    if (buffer_size_ < required_size || !buffer_) {
        // allocate only if preview buffer is not enough to store
        buffer_size_ = required_size;
        buffer_.reset(new unsigned char[buffer_size_], [](unsigned char* b) { delete[] b;});
    }

    buffer_usage_ = required_size;
    cv::Mat target(nw, nh, CV_8UC3, buffer_.get());
    cv::Mat source(*h, *w, CV_8UC3, const_cast<unsigned char *>(*buffer));

    cv::resize(source, target, target.size(), CV_INTER_LINEAR);

    *w = nw;
    *h = nh;
    *buffer = buffer_.get();
}

}  // namespace vcutter
