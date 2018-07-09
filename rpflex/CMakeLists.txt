# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.10.2)

get_filename_component(RPPLUGINS_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugins_${RPPLUGINS_ID}
    VERSION 0.1.0
    DESCRIPTION "Plugin for NVIDIA FleX library"
    LANGUAGES CXX
)

# === configure ====================================================================================
set(RPPLUGINS_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGINS_ID}")

# === plugin specific packages ===
if(NOT TARGET NvFlex::CUDA)
    set(NvFlex_ROOT "" CACHE PATH "Hint for finding NVIDIA flex root directory")
    find_package(NvFlex REQUIRED)
endif()

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
include("../rpplugins_build.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE NvFlex::CUDA)
target_link_libraries(${RPPLUGINS_ID} INTERFACE NvFlex::CUDA)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugins_install.cmake")
# ==================================================================================================
