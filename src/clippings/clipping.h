
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_H_
#define SRC_CLIPPINGS_CLIPPING_H_

#include <inttypes.h>
#include <string>
#include <list>

#include <jsoncpp/json/json.h>

#include "src/wrappers/video_player.h"
#include "src/clippings/clipping_key.h"

namespace vcutter {

class Clipping {
 public:
    explicit Clipping(const Json::Value * root);
    Clipping(const char *path, bool path_is_video);
    uint32_t w();
    uint32_t h();
    void w(uint32_t value);
    void h(uint32_t value);
    PlayerWrapper *player();
    bool good();
    void add(const ClippingKey & key);
    ClippingKey at(uint32_t frame);
    void save(const char *path);
    Json::Value serialize();
    uint32_t req_buffer_size();
    const std::list<ClippingKey> keys() const;
    ClippingKey current_key();
    void render(ClippingKey key, uint32_t target_w, uint32_t target_h, uint8_t *buffer);
    void render(ClippingKey key, uint8_t *buffer);
    void render(ClippingKey key, uint8_t *player_buffer, uint8_t *buffer);
    void render_transparent(ClippingKey key, uint8_t *player_buffer, uint8_t *buffer);

    void remove(uint32_t frame);
    void remove_before(uint32_t frame); // cutoff from begin
    void remove_after(uint32_t frame);  // cutoff from end
    void remove_others(uint32_t frame_to_keep); // cutoff_center
    void remove_all(); // clear

    uint32_t first_frame();
    uint32_t last_frame();
    void wh(uint32_t w, uint32_t h);

    ClippingKey at_index(uint32_t index);
    int find_index(uint32_t frame);

    std::string video_path();
    uint64_t version();

    void positionate_left(uint32_t frame);
    void positionate_right(uint32_t frame);
    void positionate_top(uint32_t frame);
    void positionate_bottom(uint32_t frame);
    void positionate_vertical(uint32_t frame);
    void positionate_horizontal(uint32_t frame);
    void normalize_scale(uint32_t frame);
    void align_left(uint32_t frame);
    void align_right(uint32_t frame);
    void align_top(uint32_t frame);
    void align_bottom(uint32_t frame);
    void align_all(uint32_t frame);

    // TODO: remove reference from here
    bool has_ref();
    bool get_reference_frame(int *frame);
    bool get_reference(int *frame, float *rx1, float *ry1, float *rx2, float *ry2);
    void set_reference(int frame, float rx1, float ry1, float rx2, float ry2);
    void clear_reference();

 private:
    void render(ClippingKey key, uint8_t *source_buffer, uint32_t target_w, uint32_t target_h, uint8_t *buffer, bool transparent);
    void load_json(const Json::Value & root);
    void load_file(const char *path);
    void video_open(const char *path);
    void inc_version();
    ClippingKey compute_interpolation(uint32_t frame);

 private:
    // TODO: remove reference from here
    bool has_ref_;
    int ref_frame_;
    double rx1_;
    double ry1_;
    double rx2_;
    double ry2_;

 private:
    std::string video_path_;
    int64_t version_;
    uint32_t output_w_;
    uint32_t output_h_;
    std::list<ClippingKey> keys_;
    std::unique_ptr<PlayerWrapper> player_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_H_
