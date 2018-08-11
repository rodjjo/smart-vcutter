/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_COMMON_UTILS_H_
#define SMART_VCUTTER_COMMON_UTILS_H_

#include <string>

namespace vcutter {

/*
    keeps the filename and replace paths directory and extension.
    if checkseq is true sequencial number is appended.

    the function try to generate a filename that does not exist.
*/
std::string change_filepath_dir(
    const char *source_path,
    const char *directory,
    const char *new_extension,
    bool checkseq=true);

/*
    return true if the paths exists
*/
bool filepath_exists(const char *path);

/*
    Generate a path that not exists.
    Index is the start number added to the name and it incrases until generated file exists.
*/
std::string generate_path(
    const char *prefix,
    const char *sufix,
    int start_number);

/*
    Erase a file from storage
*/
bool remove_file(const char *filename);

/*
    convert string to time in milliseconds
*/
int str_to_milliseconds(const char *timestr);

/*
    Generate a temp path to the file name
*/
std::string temp_filepath(const char *filename);

/*
    convert the time in milliseconds to str hh::mm:ss,ms
*/
void seconds_to_str(char *buffer, int size, double tmr_in_seconds, bool ms=false);

}  // namespace vcutter

#endif  // SMART_VCUTTER_COMMON_UTILS_H_
