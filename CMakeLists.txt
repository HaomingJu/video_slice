cmake_minimum_required(VERSION 2.8)


include(cmake/hobot_util.cmake)
project(videoslice)

message("config types: ${CMAKE_CONFIGURATION_TYPES}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -pthread -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}  -pthread -fPIC")
set(CMAKE_INSTALL_PREFIX ${_INSTALL_DIR})
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions(-DHR_POSIX)
add_definitions(-DHR_LINUX)
add_definitions(-D${_PLATFORM})

include_directories(
        include
        include/dms_frame
        include/protobuf_ops
        include/serializer
        include/logging
        include/time_utils
        include/search_path
)

set(SOURCE_FILES
        src/protobuf_ops/protobuf_ops.cc
        src/dms_frame/DMSFrame.pb.cc
        src/serializer/serializer.cc
        src/time_utils/TimeUtils.cc
        src/search_path/searchMP4.cc
        src/video_utils/cut_merge_video.c
        src/slice_video_proto.cc
        )

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

if (${_PLATFORM} STREQUAL "android")
    set(LOG_LIB log)
else()
    set(LOG_LIB "")
endif()

target_link_libraries(${PROJECT_NAME}
    protobuf
    hobotlog
    avformat
    avcodec
    avutil
    ${LOG_LIB}
    )

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME}
    DESTINATION lib)
