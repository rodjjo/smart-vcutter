file(GLOB Sources
    "*.cpp"
    "clippings/*.cpp"
    "common/*.cpp"
    "wrappers/*.cpp"
    "data/*.cpp"
    "wnd_common/*.cpp"
    "viewer/*.cpp"
    "viewer/editor/*.cpp"
    "wnd_cutter/*.cpp"
    "wnd_main/*.cpp"
    "wnd_tools/*.cpp")

if(WIN32)
add_executable(smart-vcutter WIN32 ${Sources})
else(WIN32)
add_executable(smart-vcutter ${Sources})
endif(WIN32)

target_include_directories (smart-vcutter PUBLIC ${PROJECT_SOURCE_DIR}/cam_finder)

add_dependencies(smart-vcutter video-stream)

target_link_libraries(smart-vcutter
                      video-stream
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_CHRONO_LIBRARY}
                      ${Boost_TIMER_LIBRARY}
                      ${AVCODEC_LIBRARY}
                      ${FLTK_LIBRARIES}
                      ${AVCODEC_LIBRARY}
                      ${FLTK_LIBRARIES}
                      ${AVCODEC_LIBRARY}
                      ${AVFORMAT_LIBRARY}
                      ${AVUTIL_LIBRARY}
                      ${AVDEVICE_LIBRARY}
                      ${SWRESAMPLE_LIBRARY}
                      ${SWSCALE_LIBRARY}
                      ${OCV_CALIB_LIBRARY}
                      ${OCV_CORE_LIBRARY}
                      ${OCV_FEATURES_LIBRARY}
                      ${OCV_FLANN_LIBRARY}
#                     ${OCV_HIGHGUI_LIBRARY}
                      ${OCV_IMGPROC_LIBRARY}
                      ${OCV_ML_LIBRARY}
                      ${OCV_OBJDETECT_LIBRARY}
                      ${OCV_PHOTO_LIBRARY}
                      ${OCV_STITCHING_LIBRARY}
                      ${OCV_SUPERRES_LIBRARY}
                      ${OCV_IMGCODECS_LIBRARY}
                      ${OCV_SHAPE_LIBRARY}
                      ${OCV_VIDEO_LIBRARY}
                      ${JSONCPP_LIBRARY}
                      )



SET_TARGET_PROPERTIES(smart-vcutter PROPERTIES LINKER_LANGUAGE C)
