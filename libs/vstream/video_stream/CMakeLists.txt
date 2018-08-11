file(GLOB Sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(VIDEOSTREAM_HEADERS "vstream/video_stream.h")

add_library(video-stream STATIC ${Sources})

SET_TARGET_PROPERTIES(video-stream PROPERTIES LINKER_LANGUAGE C)

install(TARGETS video-stream
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        COMPONENT library)

install(FILES ${VIDEOSTREAM_HEADERS}
        DESTINATION include/vstream)
