# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(flex)

# === configure ====================================================================================
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# === plugin specific packages ===
# find flex
set(FLEX_ROOT "" CACHE PATH "Hint for finding NVIDIA flex root directory")
set(FLEX_INCLUDEDIR "" CACHE PATH "Hint for finding NVIDIA flex include directory")
set(FLEX_LIBRARYDIR "" CACHE PATH "Hint for finding NVIDIA flex library directory")
find_package(Flex REQUIRED)
if(Flex_FOUND)
    include_directories(${Flex_INCLUDE_DIRS})
    message(STATUS "NVIDIA flex include path: ${Flex_INCLUDE_DIRS}")
endif()
# ================================

# configure plugin
set(LIB_NAME "plugin")
set(LIB_TYPE MODULE)
add_definitions(-DPLUGIN_ID_STRING="${PROJECT_NAME}")

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug Release."
            FORCE)
    endif()
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    endif()
else()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()

# include header directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")      # /
include_directories("${PROJECT_BINARY_DIR}")            # /build
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")  # ./src
# ==================================================================================================

# === sources ======================================================================================
# set input files
set(plugin_sources
    "${PROJECT_SOURCE_DIR}/include/flex_plugin.hpp"
    "${PROJECT_SOURCE_DIR}/src/flex_plugin.cpp"
)
# ==================================================================================================

# === build & install===============================================================================
# create library
add_library(${PROJECT_NAME} ${LIB_TYPE} ${plugin_sources})
target_link_libraries(${PROJECT_NAME}
    PUBLIC ${Boost_LIBRARIES} ${Panda3d_LIBRARIES}
    PRIVATE ${RenderPipeline_LIBRARIES} ${Flex_LIBRARIES}
)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})

# library
install(TARGETS ${PROJECT_NAME} DESTINATION "${RPPLUGINS_INSTALL_DIR}/${PROJECT_NAME}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/config/" DESTINATION "${RPPLUGINS_INSTALL_DIR}/${PROJECT_NAME}")
if(EXISTS "${PROJECT_SOURCE_DIR}/include")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" DESTINATION "include/render_pipeline/${RPPLUGINS_FOLDER_NAME}/${PROJECT_NAME}")
endif()
# ==================================================================================================
