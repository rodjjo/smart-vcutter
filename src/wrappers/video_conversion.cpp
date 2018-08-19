/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <Fl/Fl.H>

#include "src/common/utils.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/wnd_common/progress_window.h"

#include "src/wrappers/video_conversion.h"

/*
    TODO(Rodrigo): Remove this file in version 1.2.0

    The conversion process will be refactored.
*/


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
    clipping_start_at_end_ = false;
    start_frame_ = start_frame;
    end_frame_ = end_frame;
    clipping_.reset(new Clipping(source_path, true));
    init(codec_name, target_path, bitrate, fps);
}

VideoConversionWrapper::VideoConversionWrapper(
        std::shared_ptr<Clipping> clipping,
        const char* codec_name,
        const char *target_path,
        int bitrate,
        double fps,
        bool start_at_end
) {
    start_frame_ = clipping->first_frame();
    end_frame_ = clipping->last_frame();
    clipping_start_at_end_ = start_at_end;
    clipping_ = clipping;
    init(codec_name, target_path, bitrate, fps);
}

void VideoConversionWrapper::init(
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

    if (!clipping_->good()) {
        show_error("Could not open source video");
        return;
    }

    append_reverse_ = append_reverse;
    merge_frames_ = merge_reverse && clipping_ && !append_reverse;

    canceled_ = false;
    position_.store(0);
    count_.store(0);

    if (clipping_->player()->info()->error()) {
        show_error(clipping_->player()->info()->error());
        clipping_.reset();
        return;
    }

    encode_error_ = false;
    buffering_ = false;

    allocate_buffers();

    clipping_->player()->conversion_thread([this] (vs::Player *player) {
        conversion_thread(player);
    });

    auto prog_window = std::unique_ptr<ProgressWindow>(new ProgressWindow(true));

    prog_window->wait([this, prog_window{prog_window.get()}] () {
        for(;;) {
            Fl::wait(0.1);

            if (clipping_->player()->conversion_finished()) {
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
    clipping_.reset();

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

    if (clipping_) {
        return clipping_->player()->info()->buffer();
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

    if (clipping_) {
        return clipping_->player()->info()->w();
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

    if (clipping_) {
        return clipping_->player()->info()->h();
    }

    return 0;
}

uint32_t VideoConversionWrapper::interval() {
    return start_frame_ < end_frame_ ? end_frame_ - start_frame_ : start_frame_ - end_frame_;
}

void VideoConversionWrapper::allocate_buffers() {
    release_buffers();

    target_w_ = clipping_->w();
    target_h_ = clipping_->h();

    if (start_frame_ > clipping_->player()->info()->count()) {
        start_frame_ = clipping_->player()->info()->count();
    }

    if (end_frame_ > clipping_->player()->info()->count()) {
        end_frame_ = clipping_->player()->info()->count();
    }

    buffer_size_ = target_w_ * target_h_ * 3;

    auto buffer_deallocator = [] (unsigned char* data) {
        delete[] data;
    };


    if (!clipping_->keys().empty() && !append_reverse_ && (start_frame_ <= end_frame_)) {
        count_.store(interval());
            buffers_.push_back(std::shared_ptr<unsigned char>(
                new unsigned char[buffer_size_], buffer_deallocator));
        return;
    }

    const unsigned int max_memory = 256 * (1024 * 1024); // 256MB of ram of cache
    uint32_t buffered_frames = max_memory / buffer_size_;
    buffers_.reserve(buffered_frames);

    if (clipping_) {
        if (clipping_start_at_end_) {
            start_frame_ = clipping_->last_frame();
            end_frame_ = clipping_->first_frame();
        } else {
            start_frame_ = clipping_->first_frame();
            end_frame_ = clipping_->last_frame();
        }
    }

    if (start_frame_ < end_frame_ && !append_reverse_) {
        buffered_frames = 1;
    }

    if (buffered_frames > interval()) {
        buffered_frames = interval();
    }

    for (uint32_t i = 0; i < buffered_frames; ++i) {
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

void VideoConversionWrapper::keep_first_frame(vs::Player *player) {
    if (!merge_frames_) {
        first_frame_.reset();
        return;
    }
    if (first_frame_) {
        return;
    }

    if (clipping_) {
        first_key_ = clipping_->at(player->position()) ;
        unsigned int buffer_size = 3 * player->w() * player->h();
        first_frame_.reset(new unsigned char[buffer_size], [](unsigned char *data){delete[] data;});
        memcpy(first_frame_.get(), player->buffer(), buffer_size);
    }
}

void VideoConversionWrapper::encode_all(vs::Player *player, vs::Encoder *encoder) {
    int start = start_frame_ < end_frame_ ? start_frame_ : end_frame_;
    int end = start_frame_ < end_frame_ ? end_frame_ : start_frame_;

    player->seek_frame(start);
    keep_first_frame(player);

    buffer_index_.store(0);
    buffering_ = true;
    bool last_frame = false;
    int i = start;
    while (true) {
        if (canceled_)
            return;

        last_frame = i + 1 >= end;

        if (!clipping_) {
            memcpy(buffers_[i - start].get(), player->buffer(), buffer_size_);
        } else {
            clipping_->render(clipping_->at(i), buffers_[i - start].get());
            if (last_frame && merge_frames_ && first_frame_)
                clipping_->render_transparent(first_key_, first_frame_.get(), buffers_[i - start].get());
        }

        player->next();

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

void VideoConversionWrapper::encode_from_start(vs::Player *player, vs::Encoder *encoder) {
    player->seek_frame(start_frame_);
    keep_first_frame(player);

    uint32_t i = start_frame_;
    buffering_ = clipping_.get() != NULL;
    buffer_index_.store(buffering_ ? 1 : 0);
    bool last_frame = false;

    for (;;) {
        if (canceled_)
            return;

        last_frame = i + 1 >= end_frame_;

        if (clipping_) {
            clipping_->render(clipping_->at(i), buffers_[0].get());

            if (last_frame && merge_frames_ && first_frame_)
                clipping_->render_transparent(first_key_, first_frame_.get(), buffers_[0].get());

            encoder->frame(buffers_[0].get());
        } else {
            encoder->frame(player->buffer());
        }

        ++position_;

        player->next();

        ++i;
        if (i >= end_frame_)
            break;
    }
}

void VideoConversionWrapper::encode_from_end(vs::Player *player, vs::Encoder *encoder) {
    uint32_t buffer_start = start_frame_ - (buffers_.size() - 1);
    uint32_t buffer_usage = buffers_.size();
    uint32_t last_usage = 0;

    if (buffer_start <= end_frame_) {
        buffer_usage -= -buffer_usage;
        buffer_start = end_frame_;
    }

    int last_start = buffer_start;
    player->seek_frame(last_start);
    keep_first_frame(player);

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
            if (!clipping_) {
                memcpy(buffers_[buffer_index_.load()].get(), player->buffer(), buffer_size_);
            } else {
                clipping_->render(clipping_->at(player->position()), buffers_[buffer_index_.load()].get());
                if (last_loop && merge_frames_ && buffer_usage == 1 && first_frame_)
                    clipping_->render_transparent(first_key_, first_frame_.get(), buffers_[buffer_index_.load()].get());
            }

            ++buffer_index_;
            --buffer_usage;

            if (buffer_usage > 0)
                player->next();
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
        player->seek_frame(last_start);
    }
}

void VideoConversionWrapper::conversion_thread(vs::Player *player) {
    auto encoder = vs::encoder(
        codec_name_.c_str(),
        target_path_.c_str(),
        clipping_->w(),
        clipping_->h(),
        1000,
        fps_ * 1000,
        bitrate_);

    if (encoder->error()) {
        encode_error_ = true;
        return;
    }

    if (interval() <= buffers_.size() && buffers_.size() > 1) {
        encode_all(player, encoder.get());
        return;
    }

    if (start_frame_ <= end_frame_) {
        encode_from_start(player, encoder.get());
    } else {
        encode_from_end(player, encoder.get());
    }

    if (append_reverse_) {
        auto tmp = start_frame_;
        start_frame_ = end_frame_;
        end_frame_ = tmp;

        if (start_frame_ <= end_frame_) {
            encode_from_start(player, encoder.get());
        } else {
            encode_from_end(player, encoder.get());
        }
    }
}

}  // namespace vcutter
