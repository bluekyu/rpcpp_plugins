# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)

get_filename_component(RPPLUGINS_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
project(rpplugins_${RPPLUGINS_ID}
    VERSION 0.1.0
    DESCRIPTION "Plugin for OpenVR library"
    LANGUAGES CXX
)

# === configure ====================================================================================
set(RPPLUGINS_INSTALL_DIR "${render_pipeline_PLUGIN_DIR}/${RPPLUGINS_ID}")

# === plugin specific packages ===
include(windows_utils)

find_package(Boost REQUIRED filesystem)

find_package(fmt CONFIG REQUIRED)
if(TARGET fmt::fmt-header-only)                 # for libfmt in ubuntu package
    set(FMT_TARGET fmt::fmt-header-only)
else()
    set(FMT_TARGET fmt::fmt)
endif()

if(NOT TARGET OpenVR::OpenVR)
    find_package(OpenVR REQUIRED)
endif()

set(${PROJECT_NAME}_MACRO_CMAKE_FILE "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-macro.cmake")
include(${${PROJECT_NAME}_MACRO_CMAKE_FILE} OPTIONAL)
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
include("../rpplugins_build.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE OpenVR::OpenVR ${FMT_TARGET}
    $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::filesystem
)
target_link_libraries(${RPPLUGINS_ID} INTERFACE OpenVR::OpenVR)

if(MSVC)
    # See https://developercommunity.visualstudio.com/content/problem/330769/compilation-error-occured-by-some-stdmaxmin-expres.html
    if((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.15") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.16"))
        get_target_property(compile_opts ${PROJECT_NAME} COMPILE_OPTIONS)
        list(FILTER compile_opts EXCLUDE REGEX "/permissive-")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_OPTIONS "${compile_opts}")
    endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    windows_add_delay_load(TARGET ${PROJECT_NAME} IMPORTED_TARGETS OpenVR::OpenVR)
endif()
# ==================================================================================================

# === install ======================================================================================
include("../rpplugins_install.cmake")
# ==================================================================================================

# ==================================================================================================
render_pipeline_find_plugins("imgui;rpstat")
if((TARGET rpplugins::imgui) AND (TARGET rpplugins::rpstat))
    add_subdirectory("tools/gui")
endif()
# ==================================================================================================
