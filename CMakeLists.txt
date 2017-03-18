# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(rpcpp-plugins)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# check package
include(FindPackages)

set(RPPLUGINS_INSTALL_DIR "etc/render_pipeline/rpplugins")
set(RPPLUGINS_FOLDER_NAME "rpplugins")

# subdirectory
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/flex")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/rpeditor_server")
# ==================================================================================================
