
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_DATA_H_
#define SRC_CLIPPINGS_CLIPPING_DATA_H_

#include <inttypes.h>
#include <string>
#include <list>

#include <boost/core/noncopyable.hpp>
#include <jsoncpp/json/json.h>

#include "src/wrappers/video_player.h"
#include "src/clippings/clipping_key.h"

namespace vcutter {

class ClippingData: private boost::noncopyable {
 public:
    explicit ClippingData(const Json::Value * root);
    explicit ClippingData(const char *path);
    ClippingData();
    virtual ~ClippingData();
    uint32_t w();
    uint32_t h();
    void w(uint32_t value);
    void h(uint32_t value);
    void add(const ClippingKey & key);
    ClippingKey at(uint32_t frame);
    void save(const char *path);
    Json::Value serialize();
    uint32_t req_buffer_size();
    const std::list<ClippingKey> keys() const;
    void remove(uint32_t frame);
    void remove_after(uint32_t frame);  // cutoff from end
    void remove_others(uint32_t frame_to_keep); // cutoff_center
    void remove_all(); // clear
    uint32_t first_frame();
    virtual uint32_t last_frame();
    void wh(uint32_t w, uint32_t h);
    ClippingKey at_index(uint32_t index);
    int find_index(uint32_t frame);
    std::string video_path();
    uint64_t version();

 protected:
    void video_path(const char *value);
    void inc_version();
    virtual uint32_t default_w() = 0;
    virtual uint32_t default_h() = 0;

 private:
    void load_json(const Json::Value & root);
    void load_file(const char *path);
    ClippingKey compute_interpolation(uint32_t frame);

 private:
    int64_t version_;
    uint32_t output_w_;
    uint32_t output_h_;
    std::string video_path_;
 protected:
    std::list<ClippingKey> keys_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_DATA_H_
