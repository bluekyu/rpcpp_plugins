# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)

get_filename_component(RPPLUGIN_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugin_${RPPLUGIN_ID})

# === configure ====================================================================================
set(RPPLUGIN_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGIN_ID}")

set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# === plugin specific packages ===
if(NOT TARGET OpenVR)
    # find OpenVR
    set(OpenVR_ROOT "" CACHE PATH "Hint for finding OpenVR root directory")
    find_package(OpenVR REQUIRED)
endif()

if(NOT TARGET spdlog::spdlog)
    find_package(spdlog REQUIRED)
endif()

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
include("../rpplugin_build.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE OpenVR spdlog::spdlog)
target_link_libraries(${RPPLUGIN_ID} INTERFACE OpenVR)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugin_install.cmake")
# ==================================================================================================
