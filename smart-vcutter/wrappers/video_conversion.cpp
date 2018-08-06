/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <Fl/Fl.H>

#include "smart-vcutter/common/utils.h"
#include "smart-vcutter/wnd_common/common_dialogs.h"
#include "smart-vcutter/wnd_common/progress_window.h"
#include "smart-vcutter/clippings/clipping_keeper.h"

#include "smart-vcutter/wrappers/video_conversion.h"


namespace vcutter {

bool check_existence(bool should_check, const char *path) {
    if (should_check && filepath_exists(path) && !ask("The destination filepath already exists. override it ?")) {
        return false;
    }

    return true;
}

VideoConversionWrapper::VideoConversionWrapper(
        const char *source_path,
        unsigned int start_frame,
        unsigned int end_frame,
        const char* codec_name,
        const char *target_path,
        int bitrate,
        double fps
) {
    has_clipping_ = false;
    clipping_start_at_end_ = false;
    start_frame_ = start_frame;
    end_frame_ = end_frame;
    init(source_path, codec_name, target_path, bitrate, fps);
}

VideoConversionWrapper::VideoConversionWrapper(
        const clipping_t & clipping,
        const char* codec_name,
        const char *target_path,
        int bitrate,
        double fps,
        bool start_at_end
) {
    start_frame_ = 0;
    end_frame_ = 0;
    has_clipping_ = true;
    clipping_start_at_end_ = start_at_end;
    clipping_ = clipping;
    init(clipping_.video_path.c_str(), codec_name, target_path, bitrate, fps);
}

void VideoConversionWrapper::init(
        const char *source_path,
        const char* codec_name,
        const char *target_path,
        int bitrate,
        double fps
) {
    canceled_ = false;
    position_.store(0);
    count_.store(1);
    target_w_ = 0;
    target_h_ = 0;
    buffer_size_ = 0;
    buffering_ = false;
    encode_error_ = false;
    buffer_index_.store(0);
    source_path_ = source_path;
    codec_name_ = codec_name;
    target_path_ = target_path;
    bitrate_ = bitrate;
    fps_ = fps;
    append_reverse_ = false;
    merge_frames_ = false;
}


void VideoConversionWrapper::convert(bool append_reverse, bool merge_reverse, bool ask_exists) {
    if (!check_existence(ask_exists, target_path_.c_str())) {
        return;
    }

    append_reverse_ = append_reverse;
    merge_frames_ = merge_reverse && has_clipping_ && !append_reverse;

    canceled_ = false;
    position_.store(0);
    count_.store(0);

    player_ = vs::open_file(source_path_.c_str());

    if (player_->error()) {
        show_error(player_->error());
        player_.reset();
        return;
    }

    encode_error_ = false;
    buffering_ = false;

    allocate_buffers();

    thread_.reset(new boost::thread(
        [this] () {
            conversion_thread();
        }
    ));

    auto prog_window = std::unique_ptr<ProgressWindow>(new ProgressWindow(true));

    prog_window->wait([this, prog_window{prog_window.get()}] () {
        for(;;) {
            Fl::wait(0.1);

            if (thread_->timed_join(boost::posix_time::milliseconds(5))) {
                break;
            }

            if (canceled_ || prog_window->canceled()) {
                canceled_ = true;
                continue;
            }

            prog_window->set_progress(position_.load(), count_.load());
            prog_window->set_buffer(preview_buffer(), preview_w(), preview_h());
        }

        return !canceled_;
    });

    prog_window.reset();
    player_.reset();

    release_buffers();

    if (encode_error_) {
        show_error("Error encoding the video file");
    }
}


const unsigned char* VideoConversionWrapper::preview_buffer() {
    if (buffering_) {
        int index = buffer_index_.load();
        if (buffers_.size() && index > 0) {
            return buffers_[index - 1].get();
        }
    }
    if (player_) {
        return player_->buffer();
    }

    return NULL;
}

int VideoConversionWrapper::preview_w() {
    if (buffering_) {
        int index = buffer_index_.load();
        if (buffers_.size() && index > 0) {
            return target_w_;
        }
    }

    if (player_) {
        return player_->w();
    }

    return 0;
}

int VideoConversionWrapper::preview_h() {
    if (buffering_) {
        int index = buffer_index_.load();

        if (buffers_.size() && index > 0) {
            return target_h_;
        }
    }

    if (player_) {
        return player_->h();
    }

    return 0;
}

int VideoConversionWrapper::interval() {
    return start_frame_ < end_frame_ ? end_frame_ - start_frame_ : start_frame_ - end_frame_;
}

void VideoConversionWrapper::allocate_buffers() {
    release_buffers();

    target_w_ = player_->w();
    target_h_ = player_->h();

    if (start_frame_ > player_->count()) {
        start_frame_ = player_->count();
    }

    if (end_frame_ > player_->count()) {
        end_frame_ = player_->count();
    }

    if (has_clipping_) {
        target_w_ = clipping_.w;
        target_h_ = clipping_.h;
    } else if (!append_reverse_ && (start_frame_ <= end_frame_)) {
        count_.store(interval());
        return;
    }

    const unsigned int max_memory = 256 * (1024 * 1024); // 256MB of ram of cache
    buffer_size_ = target_w_ * target_h_ * 3;
    int buffered_frames = max_memory / buffer_size_;
    buffers_.reserve(buffered_frames);

    auto buffer_deallocator = [] (unsigned char* data) {
        delete[] data;
    };

    if (has_clipping_) {
        if (clipping_.items.size() < 1) {
            clipping_key_t key;
            key.frame = 0;
            key.px = player_->w() / 2.0;
            key.py = player_->h() / 2.0;
            key.scale = 1;
            key.angle = 0;
            clipping_.items.push_back(key);
        }

        if (clipping_.items.size() < 2) {
            clipping_.items.push_back(*clipping_.items.begin());
            clipping_.items.rbegin()->frame = player_->count();
        }


        if (clipping_start_at_end_) {
            start_frame_ = clipping_.items.rbegin()->frame;
            end_frame_ = clipping_.items.begin()->frame;
        } else {
            start_frame_ = clipping_.items.begin()->frame;
            end_frame_ = clipping_.items.rbegin()->frame;
        }
    }

    if (start_frame_ < end_frame_ && !append_reverse_) {
        buffered_frames = 1;
    }

    if (buffered_frames > interval()) {
        buffered_frames = interval();
    }

    for (int i = 0; i < buffered_frames; ++i) {
        buffers_.push_back(std::shared_ptr<unsigned char>(
            new unsigned char[buffer_size_], buffer_deallocator));
    }

    count_.store(interval() * (append_reverse_ ? 2 : 1));
}

void VideoConversionWrapper::release_buffers() {
    buffers_.clear();
    buffer_index_.store(0);
}

void VideoConversionWrapper::flush_buffers(vs::Encoder *encoder, int count, bool from_start, bool discart_first) {
    int pos = discart_first ? 1 : 0;

    while (pos < count) {
        if (canceled_) {
            return;
        }
        int index = from_start ? pos : (count - (pos + 1));
        const unsigned char *buffer = buffers_[index].get();
        buffer_index_.store(index);
        buffering_ = true;

        encoder->frame(buffer);
        ++position_;

        ++pos;
    }
}

void VideoConversionWrapper::keep_first_frame() {
    if (!merge_frames_) {
        first_frame_.reset();
        return;
    }
    if (first_frame_) {
        return;
    }

    if (has_clipping_) {
        ClippingKeeper keeper(clipping_, player_->w(), player_->h());
        first_key_ = keeper.get_key(player_->position()) ;
        unsigned int buffer_size = 3 * player_->w() * player_->h();
        first_frame_.reset(new unsigned char[buffer_size], [](unsigned char *data){delete[] data;});
        memcpy(first_frame_.get(), player_->buffer(), buffer_size);
    }
}

void VideoConversionWrapper::encode_all(vs::Encoder *encoder) {
    int start = start_frame_ < end_frame_ ? start_frame_ : end_frame_;
    int end = start_frame_ < end_frame_ ? end_frame_ : start_frame_;

    std::unique_ptr<ClippingKeeper> keeper;

    if (has_clipping_) {
        keeper.reset(new ClippingKeeper(clipping_,  player_->w(), player_->h()));
    }

    player_->seek_frame(start);
    keep_first_frame();

    buffer_index_.store(0);
    buffering_ = true;
    bool last_frame = false;
    int i = start;
    while (true) {
        if (canceled_) {
            return;
        }

        last_frame = i + 1 >= end;

        if (!has_clipping_) {
            memcpy(buffers_[i - start].get(), player_->buffer(), buffer_size_);
        } else {
            paint_clipping(
                player_->buffer(),
                player_->w(),
                player_->h(),
                keeper->get_key(i),
                keeper->get_width(),
                keeper->get_height(),
                buffers_[i - start].get());
            if (last_frame && merge_frames_ && first_frame_) {
                paint_clipping(
                    first_frame_.get(),
                    player_->w(),
                    player_->h(),
                    first_key_,
                    keeper->get_width(),
                    keeper->get_height(),
                    buffers_[i - start].get(),
                    true);
            }
        }

        player_->next();

        ++i;
        ++buffer_index_;
        if (i >= end)
            break;
    }

    flush_buffers(encoder, end - start, start_frame_ < end_frame_, append_reverse_);

    if (append_reverse_) {
        flush_buffers(encoder, end - start, start_frame_ > end_frame_, false);
    }
}

void VideoConversionWrapper::encode_from_start(vs::Encoder *encoder) {
    std::unique_ptr<ClippingKeeper> keeper;

    if (has_clipping_) {
        keeper.reset(new ClippingKeeper(clipping_,  player_->w(), player_->h()));
    }

    player_->seek_frame(start_frame_);
    keep_first_frame();

    int count = end_frame_ - start_frame_;
    int i = start_frame_;

    buffering_ = has_clipping_;
    buffer_index_.store(buffering_ ? 1 : 0);
    bool last_frame = false;
    for (;;) {
        if (canceled_) {
            return;
        }
        last_frame = i + 1 >= end_frame_;
        if (has_clipping_) {
            paint_clipping(
                player_->buffer(),
                player_->w(),
                player_->h(),
                keeper->get_key(i),
                keeper->get_width(),
                keeper->get_height(),
                buffers_[0].get());
                if (last_frame && merge_frames_ && first_frame_) {
                    paint_clipping(
                        first_frame_.get(),
                        player_->w(),
                        player_->h(),
                        first_key_,
                        keeper->get_width(),
                        keeper->get_height(),
                        buffers_[0].get(),
                        true);
                }
            encoder->frame(buffers_[0].get());
        } else {
            encoder->frame(player_->buffer());
        }

        ++position_;

        player_->next();

        ++i;
        if (i >= end_frame_) {
            break;
        }
    }
}

void VideoConversionWrapper::encode_from_end(vs::Encoder *encoder) {

    std::unique_ptr<ClippingKeeper> keeper;

    if (has_clipping_) {
        keeper.reset(new ClippingKeeper(clipping_,  player_->w(), player_->h()));
    }

    int buffer_start = start_frame_ - (buffers_.size() - 1);
    int start = end_frame_;
    int end = start_frame_;
    int buffer_usage = buffers_.size();
    int last_usage = 0;

    if (buffer_start <= end_frame_) {
        buffer_usage -= -buffer_usage;
        buffer_start = end_frame_;
    }

    int last_start = buffer_start;
    player_->seek_frame(last_start);
    keep_first_frame();

    bool last_loop = false;
    bool first_skippped = !append_reverse_;

    for (;;) {
        if (canceled_) {
            return;
        }

        // fill the buffer
        buffer_index_.store(0);
        buffering_ = true;

        last_usage = buffer_usage;

        while (buffer_usage > 0) {
            if (!has_clipping_) {
                memcpy(buffers_[buffer_index_.load()].get(), player_->buffer(), buffer_size_);
            } else {
                paint_clipping(
                    player_->buffer(),
                    player_->w(),
                    player_->h(),
                    keeper->get_key(player_->position()),
                    keeper->get_width(),
                    keeper->get_height(),
                    buffers_[buffer_index_.load()].get());
                if (last_loop && merge_frames_ && buffer_usage == 1 && first_frame_) {
                    paint_clipping(
                        first_frame_.get(),
                        player_->w(),
                        player_->h(),
                        first_key_,
                        keeper->get_width(),
                        keeper->get_height(),
                        buffers_[buffer_index_.load()].get(),
                        true);
                }
            }

            ++buffer_index_;
            --buffer_usage;
            if (buffer_usage > 0) {
                player_->next();
            }
        }

        // flush the buffer
        flush_buffers(encoder, last_usage, false, !first_skippped);
        first_skippped = true;

        if (last_loop) {
            break;
        }

        // compute next and usage
        buffer_usage = buffers_.size();
        buffer_start -= buffers_.size();

        if (buffer_start < end_frame_) {
            buffer_usage = last_start - end_frame_;
            if (buffer_usage <= 0) {
                break;
            }
            buffer_start = end_frame_;
            last_loop = true;
        }

        last_start = buffer_start;
        player_->seek_frame(last_start);
    }
}

void VideoConversionWrapper::conversion_thread() {
    auto encoder = vs::encoder(
        codec_name_.c_str(),
        target_path_.c_str(),
        has_clipping_ ? clipping_.w : player_->w(),
        has_clipping_ ? clipping_.h : player_->h(),
        1000,
        fps_ * 1000,
        bitrate_);

    if (encoder->error()) {
        encode_error_ = true;
        return;
    }

    if (interval() <= buffers_.size() && buffers_.size() > 1) {
        encode_all(encoder.get());
        return;
    }

    if (start_frame_ <= end_frame_) {
        encode_from_start(encoder.get());
    } else {
        encode_from_end(encoder.get());
    }

    if (append_reverse_) {
        auto tmp = start_frame_;
        start_frame_ = end_frame_;
        end_frame_ = tmp;

        if (start_frame_ <= end_frame_) {
            encode_from_start(encoder.get());
        } else {
            encode_from_end(encoder.get());
        }
    }
}

}  // namespace vcutter
