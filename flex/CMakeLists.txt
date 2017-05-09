# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(flex)

# === configure ====================================================================================
set(RPPLUGINS_INSTALL_DIR "share/render_pipeline/rpplugins/${PROJECT_NAME}")

# === plugin specific packages ===
# find flex
set(FLEX_ROOT "" CACHE PATH "Hint for finding NVIDIA flex root directory")
set(FLEX_INCLUDEDIR "" CACHE PATH "Hint for finding NVIDIA flex include directory")
set(FLEX_LIBRARYDIR "" CACHE PATH "Hint for finding NVIDIA flex library directory")
find_package(Flex REQUIRED)
# ================================

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug RelWithDebInfo Release."
            FORCE)
    endif()
    message("${PROJECT_NAME} Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif()
endif()
# ==================================================================================================

# === sources ======================================================================================
# set input files
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/include/flex_buffer.hpp"
    "${PROJECT_SOURCE_DIR}/include/flex_instance_interface.hpp"
    "${PROJECT_SOURCE_DIR}/include/flex_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/flex_buffer.cpp"
    "${PROJECT_SOURCE_DIR}/src/flex_plugin.cpp"
)
# ==================================================================================================

# === build & install===============================================================================
# create library
add_library(${PROJECT_NAME} MODULE ${plugin_sources})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RPCPP_PLUGIN_ID_STRING="${PROJECT_NAME}"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${Panda3d_INCLUDE_DIRS}
    ${Flex_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::boost
    ${Panda3d_LIBRARIES} render_pipeline
    ${Flex_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "plugin")

# library
install(TARGETS ${PROJECT_NAME} DESTINATION "${RPPLUGINS_INSTALL_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/" DESTINATION ${RPPLUGINS_INSTALL_DIR})
if(EXISTS "${PROJECT_SOURCE_DIR}/include")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION ${RPPLUGINS_INSTALL_DIR})
endif()
# ==================================================================================================
