project(videoslice)

include_directories(
        ${videoslice_SOURCE_DIR}/include
)

set(SOURCE_FILES
    slice_video_proto.cc
        )

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} slice_utils)

install(TARGETS ${PROJECT_NAME}
    DESTINATION lib)
