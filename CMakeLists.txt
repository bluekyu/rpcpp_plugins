# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(rpcpp_plugins)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# check package
include(FindPackages)

# common variable
set(RPPLUGIN_CONFIG_IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpplugin-config.cmake.in")

# plugins
set(RENDER_PIPELINE_PLUGINS "background2d" "flex" "rpeditor_server")

foreach(plugin_id ${RENDER_PIPELINE_PLUGINS})
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${plugin_id}")
endforeach()
# ==================================================================================================
