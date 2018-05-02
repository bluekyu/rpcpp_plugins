# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.9)

get_filename_component(RPPLUGIN_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugin_${RPPLUGIN_ID}
    VERSION 0.1.0
    DESCRIPTION "Plugin for OpenVR library"
)

# === configure ====================================================================================
set(RPPLUGIN_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGIN_ID}")

# === plugin specific packages ===
include(windows_utils)

_find_boost("filesystem")

if(NOT TARGET OpenVR::OpenVR)
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
target_link_libraries(${PROJECT_NAME} PRIVATE OpenVR::OpenVR spdlog::spdlog
    $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::filesystem
)
target_link_libraries(${RPPLUGIN_ID} INTERFACE OpenVR::OpenVR)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    windows_add_delay_load(${PROJECT_NAME} "openvr_api.dll")
endif()
# ==================================================================================================

# === install ======================================================================================
include("../rpplugin_install.cmake")
# ==================================================================================================
