project(time)

include_directories(
        ${dataslice_SOURCE_DIR}/include
)

set(SOURCE_FILES
        main.cc
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} dataslice)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin)

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "../lib")
