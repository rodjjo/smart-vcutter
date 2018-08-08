file(GLOB Sources "test_vstream/*.cpp" "test_vcutter/*.cpp" "*.cpp")

set(OLD_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_executable(vcutter_test ${Sources})

add_test(NAME vcutter_test COMMAND vcutter_test)

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
                      gcov
                      ${CMAKE_THREAD_LIBS_INIT}
                      )

if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(vcutter_test gcov)
endif()

set_target_properties(vcutter_test PROPERTIES COMPILE_DEFINITIONS "UNIT_TEST=1")

add_custom_command(
     TARGET vcutter_test
     COMMENT "Run tests"
     POST_BUILD 
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^vcutter_test$" --output-on-failures
)

set(CMAKE_BUILD_TYPE ${OLD_BUILD_TYPE})