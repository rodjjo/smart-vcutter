file(GLOB Sources 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vstream/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/clippings/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewer/editor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wnd_common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wnd_cutter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wnd_main/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wnd_tools/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wrappers/*.cpp")

if(WIN32)
add_executable(smart-vcutter WIN32 ${Sources})
else(WIN32)
add_executable(smart-vcutter ${Sources})
endif(WIN32)

target_include_directories (smart-vcutter PUBLIC ${PROJECT_SOURCE_DIR}/cam_finder)

target_link_libraries(smart-vcutter
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
                      ${OCV_IMGPROC_LIBRARY}
                      ${OCV_ML_LIBRARY}
                      ${OCV_OBJDETECT_LIBRARY}
                      ${OCV_PHOTO_LIBRARY}
                      ${OCV_STITCHING_LIBRARY}
                      ${OCV_SUPERRES_LIBRARY}
                      ${OCV_VIDEO_LIBRARY}
                      ${JSONCPP_LIBRARY}
                      )


SET_TARGET_PROPERTIES(smart-vcutter PROPERTIES LINKER_LANGUAGE C)

install(TARGETS smart-vcutter
        RUNTIME DESTINATION bin
        COMPONENT runtime)
