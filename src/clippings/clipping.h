
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
    ClippingKey operator[] (uint32_t frame);
    void save(const char *path);
    Json::Value serialize();
    uint32_t req_buffer_size();
    const std::list<ClippingKey> keys() const;
 private:
    void load_json(const Json::Value & root);
    void load_file(const char *path);
    void video_open(const char *path);
    void inc_version();
    ClippingKey compute_interpolation(uint32_t frame);
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
