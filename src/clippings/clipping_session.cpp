/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <FL/Fl.H>
#include "src/clippings/clipping_session.h"
#include "src/common/utils.h"

namespace vcutter {

namespace {
const char *kCLIPPING_SESSION_NAME = "-clipping-session.vcutter";
}  // namespace

ClippingSession::ClippingSession(const char *session_name, const char *path, bool path_is_video, frame_callback_t frame_cb)
 : Clipping(path, path_is_video, frame_cb), session_name_(session_name) {
    Fl::add_timeout(1.0, &ClippingSession::fltk_timeout_handler, this);
}

ClippingSession::ClippingSession(const char *session_name, const Json::Value * root, frame_callback_t frame_cb)
 : Clipping(root, frame_cb), session_name_(session_name) {
}

ClippingSession::~ClippingSession() {
    Fl::remove_timeout(&ClippingSession::fltk_timeout_handler, this);
    remove_session();
}

void ClippingSession::fltk_timeout_handler(void* clipping_session) {
    static_cast<ClippingSession *>(clipping_session)->save_session();
    Fl::repeat_timeout(1.0, &ClippingSession::fltk_timeout_handler, clipping_session);
}

void ClippingSession::save_session() {
    if (last_version_ != version()) {
        last_version_ = version();
        save(session_path().c_str(), false);
    }
}

std::string ClippingSession::session_path() {
    return temp_filepath((session_name_ + kCLIPPING_SESSION_NAME).c_str());
}

std::unique_ptr<ClippingSession> ClippingSession::restore_session(const char *session_name, frame_callback_t frame_cb) {
    std::string path = temp_filepath((std::string(session_name) + kCLIPPING_SESSION_NAME).c_str());

    std::unique_ptr<ClippingSession> result(
        new ClippingSession(session_name, path.c_str(), false, frame_cb)
    );

    if (!result->good()) {
        result.reset();
    }

    return result;
}

void ClippingSession::remove_session() {
    remove_file(session_path().c_str());
}

}  // namespace vcutter
