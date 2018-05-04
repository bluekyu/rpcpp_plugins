# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.9)
project(rpcpp_plugins
    DESCRIPTION "Plugins for Render Pipeline C++"
    LANGUAGES CXX
)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(debugging-information)

# check package
include(FindPackages)

# check installing directory
if(render_pipeline_INSTALL_DIR)
    get_filename_component(_render_pipeline_INSTALL_DIR ${render_pipeline_INSTALL_DIR} ABSOLUTE)
    get_filename_component(_CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} ABSOLUTE)
    if(NOT "${_render_pipeline_INSTALL_DIR}" STREQUAL "${_CMAKE_INSTALL_PREFIX}")
        message(AUTHOR_WARNING "CMAKE_INSTALL_PREFIX is NOT same as installed directory of Render Pipeline:
        CMAKE_INSTALL_PREFIX: ${_CMAKE_INSTALL_PREFIX}
 render_pipeline_INSTALL_DIR: ${_render_pipeline_INSTALL_DIR}")
    endif()
    unset(_render_pipeline_INSTALL_DIR)
    unset(_CMAKE_INSTALL_PREFIX)
endif()

# common variable
set(RPPLUGIN_CONFIG_IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpplugin-config.cmake.in")

# plugins
set(RENDER_PIPELINE_PLUGINS "ar_render" "background2d" "openvr" "rpflex" "rpeditor_server")

foreach(plugin_id ${RENDER_PIPELINE_PLUGINS})
    option(rpcpp_plugins_BUILD_${plugin_id} "Enable to build '${plugin_id}'" OFF)
    if(rpcpp_plugins_BUILD_${plugin_id})
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${plugin_id}")
    endif()
endforeach()
# ==================================================================================================
