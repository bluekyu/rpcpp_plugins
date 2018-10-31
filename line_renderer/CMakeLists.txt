# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)
project(line_renderer
    LANGUAGES CXX
)

# === configure ====================================================================================
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# check package
#include(FindPackages)

# debug
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D")
set(CMAKE_DEBUG_POSTFIX "d")

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug Release."
            FORCE)
    endif()
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    endif()
else()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()

# add options
set(LIB_NAME "plugin")
set(LIB_TYPE MODULE)
add_definitions(-DRPCPP_PLUGIN_ID_STRING="${PROJECT_NAME}")

# subdirectory
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}")

# include header directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")      # /
include_directories("${PROJECT_BINARY_DIR}")            # /build
include_directories("${RP_PUBLIC_HEADER_DIR}/..")      # /render_pipeline/..

# set input files
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/include/line_3d.hpp"
    "${PROJECT_SOURCE_DIR}/include/line_renderer_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/line_3d_impl.cpp"
    "${PROJECT_SOURCE_DIR}/src/line_3d_impl.hpp"
    "${PROJECT_SOURCE_DIR}/src/line_render_stage.cpp"
    "${PROJECT_SOURCE_DIR}/src/line_render_stage.hpp"
    "${PROJECT_SOURCE_DIR}/src/plugin.cpp"
    "${PROJECT_SOURCE_DIR}/src/plugin.hpp"
)
# ==================================================================================================

# === build & install===============================================================================
# create library
include(plugin_build)
# ==================================================================================================
