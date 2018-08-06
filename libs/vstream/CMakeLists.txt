project(VideoStream C CXX)

cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(FLTK_SKIP_FLUID true)

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)
find_library(AVFORMAT_LIBRARY avformat)
find_library(AVUTIL_LIBRARY avutil)
find_library(AVDEVICE_LIBRARY avdevice)
find_library(SWRESAMPLE_LIBRARY swresample)
find_library(SWSCALE_LIBRARY swscale)

if(FLTK_INCLUDE_DIR)
else(FLTK_INCLUDE_DIR)
find_package(FLTK REQUIRED)
endif(FLTK_INCLUDE_DIR)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost
            COMPONENTS
            system
            filesystem
            thread
            REQUIRED)

if(LINUX)
find_package(PkgConfig REQUIRED)
endif(LINUX)

include_directories(VideoStream
                    ${PROJECT_SOURCE_DIR}
                    ${AVCODEC_INCLUDE_DIR}
                    ${FLTK_INCLUDE_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin)

set(PLATFORM_SPECIFIC_LIBS "-lpthread")

add_subdirectory("video_stream")
