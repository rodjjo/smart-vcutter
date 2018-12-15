
file(GLOB ExeSources
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/vstream/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/geometry/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/controls/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/ui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/clippings/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/data/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/viewer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/viewer/editor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/wnd_common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/wnd_cutter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/wnd_main/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/wnd_tools/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/player/*.cpp")

file(GLOB TestSources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_vcutter/common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_vcutter/clippings/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_vcutter/data/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_vstream/*.cpp")


set(TestSources ${TestSources} ${ExeSources})
set(OLD_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_executable(vcutter_test ${TestSources})

if(WIN32)
    message(STATUS "ATTENTION: ON WINDOWS COVERAGE IS DISABLED BECAUSE LCOV IS USED \n(coverage will be enabled on windows soon). 😉")
else()
    if(CMAKE_COMPILER_IS_GNUCXX)
        include(CodeCoverage)
        setup_target_for_coverage(NAME coverage EXECUTABLE vcutter_test)
        ## APPEND_COVERAGE_COMPILER_FLAGS()
        set(COVERAGE_EXCLUDES '/usr/*' '*/test/*')
    endif()
endif()

target_link_libraries(vcutter_test
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
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
#                      ${OCV_IMGCODECS_LIBRARY}
#                      ${OCV_SHAPE_LIBRARY}
                      ${OCV_VIDEO_LIBRARY}
                      ${JSONCPP_LIBRARY}
                      gcov)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(vcutter_test gcov)
endif()

set_target_properties(vcutter_test PROPERTIES COMPILE_DEFINITIONS "UNIT_TEST=1")

add_test(NAME vcutter_test COMMAND vcutter_test)

add_custom_command(
     TARGET vcutter_test
     COMMENT "Run the tests"
     POST_BUILD
     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
     COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C Debug  --verbose -R "^vcutter_test.*$"
)

set(CMAKE_BUILD_TYPE ${OLD_BUILD_TYPE})