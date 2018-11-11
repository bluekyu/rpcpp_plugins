# Author: Younguk Kim (bluekyu)

cmake_minimum_required(VERSION 3.11.4)

project(rpplugins_gui_${RPPLUGINS_ID}
    VERSION 0.1.0
    DESCRIPTION "Plugin for OpenVR library"
    LANGUAGES CXX
)

# === configure ====================================================================================
# === plugin specific packages ===
find_package(imgui CONFIG)
if(NOT TARGET imgui::imgui)
    message(STATUS "  ${PROJECT_NAME} project will be disabled.\n")
    return()
endif()
set_target_properties(imgui::imgui PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

find_package(fmt CONFIG REQUIRED)
if(TARGET fmt::fmt-header-only)                 # for libfmt in ubuntu package
    set(FMT_TARGET fmt::fmt-header-only)
else()
    set(FMT_TARGET fmt::fmt)
endif()

if(NOT TARGET OpenVR::OpenVR)
    find_package(OpenVR REQUIRED)
endif()
# ==================================================================================================

# === target =======================================================================================
include("${PROJECT_SOURCE_DIR}/files.cmake")
include("rpplugins_gui_build.cmake")
target_link_libraries(${PROJECT_NAME}
    PRIVATE $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    OpenVR::OpenVR ${FMT_TARGET}
)

if(MSVC)
    # See https://developercommunity.visualstudio.com/content/problem/330769/compilation-error-occured-by-some-stdmaxmin-expres.html
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.15")
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
install(TARGETS ${PROJECT_NAME} DESTINATION ${RPPLUGINS_INSTALL_DIR})

install(FILES ${${PROJECT_NAME}_MACRO_CMAKE_FILE} DESTINATION ${PACKAGE_CMAKE_INSTALL_DIR} OPTIONAL)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION "${RPPLUGINS_INSTALL_DIR}")
endif()

# ==================================================================================================
