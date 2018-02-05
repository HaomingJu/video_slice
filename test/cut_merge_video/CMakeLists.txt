project(cut_merge_video)

include_directories(
        ${videoslice_SOURCE_DIR}/include
)

set(SOURCE_FILES
        main.cc
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} videoslice)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin)

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "../lib")
