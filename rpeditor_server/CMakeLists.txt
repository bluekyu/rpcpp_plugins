# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(rpeditor_server)

# === configure ====================================================================================
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

set(RPPLUGINS_FOLDER_NAME "rpplugins")
set(RPPLUGINS_INSTALL_DIR "share/render_pipeline/${RPPLUGINS_FOLDER_NAME}/${PROJECT_NAME}")

# === plugin specific packages ===
# find Qt
set(CMAKE_AUTOUIC ON)               # Run uic automatically when needed.
set(CMAKE_AUTOMOC ON)               # Run moc automatically when needed.
find_package(Qt5WebSockets REQUIRED)

# find rapidjson
find_package(RapidJSON REQUIRED)

# find fmt
find_package(fmt REQUIRED)

include_directories(${Panda3d_INCLUDE_DIRS})
include_directories(${RenderPipeline_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${RAPIDJSON_INCLUDE_DIRS})
# ================================

# debug
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D")
set(CMAKE_DEBUG_POSTFIX "d")

# configure plugin
set(LIB_NAME "plugin")
set(LIB_TYPE MODULE)
add_definitions(-DRPCPP_PLUGIN_ID_STRING="${PROJECT_NAME}")

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
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")  # /src
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")  # /include

# set input files
include("${PROJECT_SOURCE_DIR}/files.cmake")
# ==================================================================================================

# === build & install===============================================================================
# create library
add_library(${PROJECT_NAME} ${LIB_TYPE} ${plugin_sources} ${plugin_headers})
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${Boost_LIBRARIES} ${Panda3d_LIBRARIES} ${RenderPipeline_LIBRARIES} Qt5::WebSockets
    fmt-header-only
)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})

# library
install(TARGETS ${PROJECT_NAME} DESTINATION "${RPPLUGINS_INSTALL_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/" DESTINATION ${RPPLUGINS_INSTALL_DIR})
if(EXISTS "${PROJECT_SOURCE_DIR}/include")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION ${RPPLUGINS_INSTALL_DIR})
endif()
# ==================================================================================================
