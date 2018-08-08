project(SmartVCutter C CXX)

cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_CXX_STANDARD 14)
set(FLTK_SKIP_FLUID true)
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)
find_library(AVFORMAT_LIBRARY avformat)


find_package(FLTK REQUIRED)
find_package(Threads REQUIRED)

find_package(Boost
            COMPONENTS
            chrono
            filesystem
            system
            timer
            thread
            unit_test_framework
            REQUIRED)

find_library(AVCODEC_LIBRARY avcodec)
find_library(AVFORMAT_LIBRARY avformat)
find_library(AVUTIL_LIBRARY avutil)
find_library(AVDEVICE_LIBRARY avdevice)
find_library(SWRESAMPLE_LIBRARY swresample)
find_library(SWSCALE_LIBRARY swscale)
find_library(JSONCPP_LIBRARY jsoncpp)

find_library(OCV_CORE_LIBRARY "opencv_core")
find_library(OCV_CALIB_LIBRARY "opencv_calib3d")
find_library(OCV_FEATURES_LIBRARY "opencv_features2d")
find_library(OCV_FLANN_LIBRARY "opencv_flann")
# find_library(OCV_HIGHGUI_LIBRARY "opencv_highgui")
find_library(OCV_IMGPROC_LIBRARY "opencv_imgproc")
find_library(OCV_ML_LIBRARY "opencv_ml")
find_library(OCV_IMGCODECS_LIBRARY "opencv_imgcodecs")
find_library(OCV_SHAPE_LIBRARY "opencv_shape")
find_library(OCV_VIDEO_LIBRARY "opencv_video")
find_library(OCV_OBJDETECT_LIBRARY "opencv_objdetect")
find_library(OCV_PHOTO_LIBRARY "opencv_photo")
find_library(OCV_STITCHING_LIBRARY "opencv_stitching")
find_library(OCV_SUPERRES_LIBRARY "opencv_superres")


if(LINUX)
find_package(PkgConfig REQUIRED)
endif(LINUX)

include_directories(SmartVCutter
                    "${PROJECT_SOURCE_DIR}/libs/vstream"
                    ${PROJECT_SOURCE_DIR}
                    ${AVCODEC_INCLUDE_DIR}
                    ${FLTK_INCLUDE_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin)

set(PLATFORM_SPECIFIC_LIBS "-lpthread")

add_subdirectory("smart-vcutter")
add_subdirectory("libs/vstream")
add_subdirectory("tests")