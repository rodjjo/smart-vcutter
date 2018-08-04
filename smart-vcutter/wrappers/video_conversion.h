#ifndef SMART_VCUTTER_WRAPPERS_VIDEO_CONVERSION_H
#define SMART_VCUTTER_WRAPPERS_VIDEO_CONVERSION_H

#include <functional>
#include <string>
#include <atomic>
#include <memory>
#include <boost/thread.hpp>
#include "video_stream/vstream/video_stream.h"
#include "smart-vcutter/data/project.h"

namespace vcutter {

class VideoConversionWrapper {
 public:
    VideoConversionWrapper(
        const char *source_path,
        unsigned int start_frame,
        unsigned int end_frame,
        const char* codec_name, 
        const char *target_path, 
        int bitrate, 
        double fps);

    VideoConversionWrapper(
        const clipping_t & clipping,
        const char* codec_name, 
        const char *target_path, 
        int bitrate, 
        double fps,
        bool start_at_end);

    void convert(bool append_reverse = false, bool merge_reverse = false, bool ask_exists = true);
 private:
    void init(
        const char *source_path,
        const char* codec_name,
        const char *target_path,
        int bitrate,
        double fps);
    void conversion_thread();
    void allocate_buffers();
    void release_buffers();
    const unsigned char* preview_buffer();
    int preview_w();
    int preview_h();
    int interval();
    void flush_buffers(vs::Encoder *encoder, int count, bool from_start, bool discart_first);
    void encode_all(vs::Encoder *encoder);
    void encode_from_start(vs::Encoder *encoder);
    void encode_from_end(vs::Encoder *encoder);
    void keep_first_frame();
 private:
    std::shared_ptr<vs::Player> player_;
    std::unique_ptr<boost::thread> thread_;
    
    std::vector<std::shared_ptr<unsigned char> > buffers_;
    int buffer_size_;
    std::atomic_int buffer_index_;

    std::string source_path_;
    std::string codec_name_;
    std::string target_path_;
    
    int target_w_;
    int target_h_;
    
    int start_frame_;
    int end_frame_;
    bool buffering_;
    bool encode_error_;
    bool canceled_;
    int bitrate_;
    double fps_;
    bool clipping_start_at_end_;
    bool append_reverse_;
    bool merge_frames_;
    clipping_key_t first_key_;
    std::shared_ptr<unsigned char> first_frame_;
    bool has_clipping_;
    clipping_t clipping_;
    std::atomic_int position_;
    std::atomic_int count_;
};

}  // namespace vcutter

#endif  // SMART_VCUTTER_TASKS_VIDEO_CONVERSIONS_H
