#ifndef SMART_VCUTTER_VIEWER_BUFFER_VIEWER_H
#define SMART_VCUTTER_VIEWER_BUFFER_VIEWER_H

#include <inttypes.h>
#include <memory>
#include <FL/Fl_Gl_Window.H>


namespace vcutter {

class BufferViewer;

class BufferSupplier {
 public:
    virtual ~BufferSupplier() {};
    virtual void viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) {};
};

class EventHandler {
  public:
    virtual ~EventHandler() {};
    virtual void viewer_mouse_cancel(BufferViewer *viewer) {};
    virtual void viewer_mouse_alt(BufferViewer *viewer) {};
    virtual void viewer_mouse_down(BufferViewer *viewer, bool left_pressed, bool right_pressed, int x, int y) {};
    virtual void viewer_mouse_move(BufferViewer *viewer, bool left_pressed, bool right_pressed, int dx, int dy, int x, int y) {};
    virtual void viewer_mouse_up(BufferViewer *viewer, bool left_pressed, bool right_pressed, int dx, int dy, int ux, int uy) {};
    virtual void viewer_after_draw(BufferViewer *viewer) {};
};

class DrawHandler: public EventHandler {
  public:
    virtual ~DrawHandler() {};
    virtual void viewer_draw(BufferViewer *viewer, bool *handled, const unsigned char* buffer, uint32_t w, uint32_t h) { 
        *handled = false;
    };
};

class BufferViewer: public Fl_Gl_Window {
 public:
    BufferViewer(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    BufferViewer(BufferSupplier *supplier, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    BufferViewer(DrawHandler *observer, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    BufferViewer(BufferSupplier *supplier, DrawHandler *observer, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    virtual ~BufferViewer();
    void cancel_operations();
    const int *view_port() const;
 private:
    void init(BufferSupplier *supplier, DrawHandler *observer);
    void draw_buffer(const unsigned char* buffer, uint32_t w, uint32_t h);
    void update_cache(const unsigned char** buffer, uint32_t *w, uint32_t *h);
 protected:
    int handle(int event) override;
    void draw() override;
    void draw_overlay() override;

 private:
    int vp_[4];
    BufferSupplier *supplier_;
    DrawHandler *observer_;
    bool mouse_down_left_;
    bool mouse_down_right_;
    int mouse_down_x_;
    int mouse_down_y_;
    unsigned int buffer_size_;
    unsigned int buffer_usage_;
    std::shared_ptr<unsigned char> buffer_;
};

} //namespace vcutter

#endif  // SMART_VCUTTER_VIEWER_BUFFER_VIEWER_H
