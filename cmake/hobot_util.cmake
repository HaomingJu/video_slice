cmake_minimum_required(VERSION 2.8)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/internal_utils.cmake)

SET(PROPERTY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build.properties")
SET(PROPERTY_FILE_LOCAL "${CMAKE_CURRENT_SOURCE_DIR}/build.properties.local")
SET(GRADLE_DEFAULT_CMD "./gradlew")
SET(GRADLE_WIN_CMD "gradlew.bat")
SET(GRADLE_CMD ${GRADLE_DEFAULT_CMD})
SET(DEBUG_POSTFIX "d")

# load properties into ${PROPERTIES}
MACRO(READ_PROPERTIES PROPERTIES)
  if ((NOT (EXISTS ${PROPERTY_FILE})) AND (NOT (EXISTS ${PROPERTY_FILE_LOCAL})))
    message(FATAL_ERROR "CONFIG FILE `${PROPERTY_FILE}` and `${PROPERTY_FILE_LOCAL}` not EXISTS")
  endif ()
  if (EXISTS ${PROPERTY_FILE})
    message(STATUS "READ CONFIG FROM FILE:${PROPERTY_FILE}")
    LOAD_PROPERTY(${PROPERTIES} ${PROPERTY_FILE})
  endif ()
  if (EXISTS ${PROPERTY_FILE_LOCAL})
    message(STATUS "READ CONFIG FROM FILE:${PROPERTY_FILE_LOCAL}")
    LOAD_PROPERTY(${PROPERTIES} ${PROPERTY_FILE_LOCAL})
  endif ()
ENDMACRO()

MACRO(PARSE_ARTIFACT_URI URI _GROUP _NAME _VERSION _EXTENSION)
  STRING(REGEX REPLACE ":" ";" fields ${URI})
  list(GET fields 0 ${_GROUP})
  list(GET fields 1 ${_NAME})
  list(GET fields 2 ${_VERSION})
  list(GET fields 3 ${_EXTENSION})
ENDMACRO()

# init build type
MACRO(INIT_TYPE PROPERTIES)
  MAP_GET(${PROPERTIES} build_type build_type)
  if (${build_type} STREQUAL "debug")
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "BUILD TYPE:Debug")
    set(HOBOT_BUILD_DEBUG "TRUE")
  else ()
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "BUILD TYPE:Release")
  endif ()
ENDMACRO()

# init platform info from ${PROPERTIES}
MACRO(INIT_PLATFORM_INFO PROPERTIES)
  MAP_GET(${PROPERTIES} platform platform)
  MAP_GET(${PROPERTIES} architecture architecture)
  set(_PLATFORM ${platform})
  set(_ARCHITECTURE ${architecture})
  if (${platform} STREQUAL "mac")
    set(PLATFORM_MAC TRUE)
  elseif (${platform} STREQUAL "linux")
    set(PLATFORM_LINUX TRUE)
    MAP_GET(${PROPERTIES} cmake_c_compiler C_COMPILER)
    MAP_GET(${PROPERTIES} cmake_cxx_compiler CXX_COMPILER)
    if (${C_COMPILER} STREQUAL "undefined")
    else ()
      SET(CMAKE_C_COMPILER ${C_COMPILER})
    endif ()
    if (${CXX_COMPILER} STREQUAL "undefined")
    else ()
      SET(CMAKE_CXX_COMPILER ${CXX_COMPILER})
    endif ()
    if (${_ARCHITECTURE} STREQUAL "default")
        set(_ARCHITECTURE "x86")
    endif ()

  elseif (${platform} STREQUAL "win")
    SET(GRADLE_CMD ${GRADLE_WIN_CMD})
    set(PLATFORM_WIN TRUE)
  elseif (${platform} STREQUAL "android")
    set(PLATFORM_ANDROID TRUE)
    set(ANDROID_STL gnustl_static)
    # set(ANDROID_STL c++_static)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    MAP_GET(${PROPERTIES} ANDROID_NDK ANDROID_NDK)
    if (${ANDROID_NDK} STREQUAL "undefined")
      message(FATAL_ERROR "ANDROID_NDK NOT SET")
    endif ()
    set(ANDROID_NDK ${ANDROID_NDK})
    # set(ANDROID_TOOLCHAIN_NAME "standalone-clang")
    # MAP_GET(${PROPERTIES} ANDROID_STANDALONE_TOOLCHAIN ANDROID_STANDALONE_TOOLCHAIN)
    # if (${ANDROID_STANDALONE_TOOLCHAIN} STREQUAL "undefined")
        # message(FATAL_ERROR "ANDROID_STANDALONE_TOOLCHAI NOT SET")
    # endif ()
    # message("wwt ${ANDROID_STANDALONE_TOOLCHAIN}")
    set(ANDROID_STANDALONE_TOOLCHAIN ${ANDROID_STANDALONE_TOOLCHAIN})
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/android.toolchain.cmake")
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    set(ANDROID_ABI armeabi-v7a)
    set(ANDROID_NATIVE_API_LEVEL android-21)
    message(STATUS "cmake -DPLATFORM_ANDROID=${PLATFORM_ANDROID} -DANDROID_STL=${ANDROID_STL} -DANDROID_NDK=${ANDROID_NDK} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DANDROID_ABI=${ANDROID_ABI} -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL} ..")
    if (${_ARCHITECTURE} STREQUAL "default")
        set(_ARCHITECTURE "arm")
    endif ()
  else ()
    message(FATAL_ERROR "platform `${platform}` not suppored, choose from [linux | android | win | mac]")
  endif ()
ENDMACRO()

# init dependency info from ${PROPERTIES}
MACRO(INIT_DEPENDENCY_INFO)
  execute_process(
      COMMAND ${GRADLE_CMD} --daemon solveDependencies
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE RESULT
      OUTPUT_VARIABLE OUTPUT
  )
  if ((${architecture} STREQUAL "default")
      OR (${architecture} STREQUAL "x86" AND ${platform} STREQUAL "linux")
      OR (${architecture} STREQUAL "arm" AND ${platform} STREQUAL "android"))
    set(dep_file "${CMAKE_CURRENT_SOURCE_DIR}/dep_${platform}.tmp")
  else ()
    set(dep_file "${CMAKE_CURRENT_SOURCE_DIR}/dep_${platform}_${architecture}.tmp")
  endif ()
  message(STATUS "dep file path:${dep_file}")
  GETLINES(lines ${dep_file})
  foreach (line ${lines})
    set(INCLUDE_SEARCH_PATH "${line}/include")
    set(LINK_SEARCH_PATH "${line}/lib")
    include_directories(${INCLUDE_SEARCH_PATH})
    message(STATUS "ADD HEADER SEARCH PATH:${INCLUDE_SEARCH_PATH}")
    link_directories(${LINK_SEARCH_PATH})
    message(STATUS "ADD LIBRARY SEARCH PATH:${LINK_SEARCH_PATH}")
  endforeach ()

  set(CMAKE_DEBUG_POSTFIX ${DEBUG_POSTFIX})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  if (${PLATFORM_WIN})
    set(RUNTIME_DEPS "${CMAKE_BINARY_DIR}/bin/$(Configuration)")
    link_directories(${CMAKE_BINARY_DIR}/lib/$(Configuration))
  else ()
    set(RUNTIME_DEPS "${CMAKE_BINARY_DIR}/bin/")
    link_directories(${CMAKE_BINARY_DIR}/lib)
  endif ()
  set(_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/output_${_PLATFORM}_${_ARCHITECTURE}/")
  set(LIB_DEPS "${CMAKE_CURRENT_SOURCE_DIR}/output_${_PLATFORM}_${_ARCHITECTURE}/lib/")
  set(MODEL_DEPS "${CMAKE_CURRENT_SOURCE_DIR}/output_${_PLATFORM}_${_ARCHITECTURE}/etc/models/")
  set(RES_DEPS "${CMAKE_CURRENT_SOURCE_DIR}/output_${_PLATFORM}_${_ARCHITECTURE}/etc/res/")
  set(DATA_DEPS "${CMAKE_CURRENT_SOURCE_DIR}/output_${_PLATFORM}_${_ARCHITECTURE}/data/")

ENDMACRO()


READ_PROPERTIES(properties)
INIT_TYPE(properties)
INIT_PLATFORM_INFO(properties)
INIT_DEPENDENCY_INFO()

function(hobot_link_libraries)
  foreach (lib ${ARGN})
    link_libraries(debug "${lib}${DEBUG_POSTFIX}" optimized ${lib})
  endforeach ()
endfunction()

function(hobot_target_link_libraries target)
  foreach (lib ${ARGN})
    target_link_libraries(${target} debug "${lib}${DEBUG_POSTFIX}" optimized ${lib})
  endforeach ()
endfunction()


# show dependency tree
add_custom_target(deps
    COMMAND ${GRADLE_CMD} --daemon dependencies --configuration compile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# upload archive
add_custom_target(upload
    COMMAND ${GRADLE_CMD} --daemon uploadArchive
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# copy dependencies (shared libraries and models)
add_custom_target(copy
    COMMAND ${GRADLE_CMD} --daemon copyRuntime -Pdest=${LIB_DEPS}
    COMMAND ${GRADLE_CMD} --daemon copyModels -Pdest=${MODEL_DEPS}
    COMMAND ${GRADLE_CMD} --daemon copyResources -Pdest=${RES_DEPS}
    COMMAND ${GRADLE_CMD} --daemon copyRuntimeData -Pdest=${DATA_DEPS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})



