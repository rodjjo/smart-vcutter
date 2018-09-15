
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_SESSION_H_
#define SRC_CLIPPINGS_CLIPPING_SESSION_H_

#include <inttypes.h>
#include <string>
#include <memory>
#include "src/clippings/clipping.h"

namespace vcutter {

/*
    Create a session to recovery user's work after an unexpected app crash.
*/
class ClippingSession: public Clipping {
 public:
    explicit ClippingSession(const char *session_name, const Json::Value * root);
    ClippingSession(const char *session_name, const char *path, bool path_is_video);
    virtual ~ClippingSession();
    static void fltk_timeout_handler(void* clipping_session);
    static std::unique_ptr<ClippingSession> restore_session(const char *session_name);
 private:
    std::string session_path();
    void save_session();
    void remove_session();
 private:
    uint64_t last_version_;
    std::string session_name_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_SESSION_H_
