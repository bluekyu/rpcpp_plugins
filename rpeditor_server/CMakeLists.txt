# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.9)

get_filename_component(RPPLUGIN_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugin_${RPPLUGIN_ID}
    VERSION 0.1.0
    LANGUAGES CXX
)

# === configure ====================================================================================
set(RPPLUGIN_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGIN_ID}")

# === plugin specific packages ===
if(NOT TARGET Qt5::WebSockets)
    # find Qt
    set(CMAKE_AUTOUIC ON)               # Run uic automatically when needed.
    set(CMAKE_AUTOMOC ON)               # Run moc automatically when needed.
    find_package(Qt5WebSockets REQUIRED)
endif()

find_package(RapidJSON REQUIRED)
find_package(fmt REQUIRED)

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
include("../rpplugin_build.cmake")
target_include_directories(${PROJECT_NAME} PRIVATE ${RAPIDJSON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::WebSockets fmt-header-only)
# ==================================================================================================

# === install ======================================================================================
include("../rpplugin_install.cmake")
# ==================================================================================================
