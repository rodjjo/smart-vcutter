/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_UI_DIALOGS_FILE_DIALOG_H_
#define SRC_UI_DIALOGS_FILE_DIALOG_H_

#include <string>

namespace vcutter {

typedef enum {
    filedialog_project = 0,
    filedialog_video,
    filedialog_project_or_video,
    filedialog_webm,
    filedialog_mp4,
    filedialog_mjpeg
} filedialog_type;

class FileDialog {
 public:
    FileDialog(filedialog_type dialog_type, bool opening);
    virtual ~FileDialog();
    std::string choose_path();

 private:
    void load_history();
    void save_history();
    const char *history_key();
    const char *filter();
    const char *title();

 private:

    filedialog_type dialog_type_;
    std::string history_;
    bool opening_;
};

}  // namespace vcutter

#endif  // SRC_UI_DIALOGS_FILE_DIALOG_H_
