/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "video_stream/vstream/video_stream.h"
#include "video_stream/player.h"
#include "video_stream/encoder.h"

namespace vs {

// abastract classes destructors:

StreamInfo::~StreamInfo() {}
Player::~Player() {}
Encoder::~Encoder() {}
// instance creating functions:

std::shared_ptr<vs::Player> open_file(const char* path) {
    return std::shared_ptr<vs::Player>(new vs::PlayerImp(path, vs::file_source));
}

std::shared_ptr<Encoder> encoder(
    const char *codec_name,
    const char *path,
    unsigned int frame_width,
    unsigned int frame_height,
    int fps_numerator,
    int fps_denominator,
    int bit_rate
) {
    return std::shared_ptr<vs::Encoder>(new vs::EncoderImp(
        codec_name,
        path,
        frame_width,
        frame_height,
        fps_numerator,
        fps_denominator,
        bit_rate
    ));
}

void initialize() {
    av_register_all();  // linux needs
    avformat_network_init();
}

}  // namespace vs
