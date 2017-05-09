# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(rpeditor_server)

# === configure ====================================================================================
set(RPPLUGINS_INSTALL_DIR "share/render_pipeline/rpplugins/${PROJECT_NAME}")

# === plugin specific packages ===
# find Qt
set(CMAKE_AUTOUIC ON)               # Run uic automatically when needed.
set(CMAKE_AUTOMOC ON)               # Run moc automatically when needed.
find_package(Qt5WebSockets REQUIRED)

# find rapidjson
find_package(RapidJSON REQUIRED)

# find fmt
find_package(fmt REQUIRED)
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

# set input files
include("${PROJECT_SOURCE_DIR}/files.cmake")
# ==================================================================================================

# === build & install===============================================================================
# create library
add_library(${PROJECT_NAME} MODULE ${plugin_sources} ${plugin_headers})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RPCPP_PLUGIN_ID_STRING="${PROJECT_NAME}"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${Panda3d_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE $<$<NOT:$<BOOL:${Boost_USE_STATIC_LIBS}>>:Boost::dynamic_linking>
    Boost::boost
    ${Panda3d_LIBRARIES} render_pipeline Qt5::WebSockets fmt-header-only
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "plugin")

# library
install(TARGETS ${PROJECT_NAME} DESTINATION "${RPPLUGINS_INSTALL_DIR}/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/" DESTINATION ${RPPLUGINS_INSTALL_DIR})
if(EXISTS "${PROJECT_SOURCE_DIR}/include")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION ${RPPLUGINS_INSTALL_DIR})
endif()
# ==================================================================================================
