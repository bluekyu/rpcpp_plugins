#.rst:
# FindRapidJson
# --------------
#
# FindRapidJson.cmake
#
# Author: Younguk Kim (bluekyu)
# Date  : 2017-01-18
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   RapidJson_INCLUDE_DIRS     - Include directories for RapidJSON
#   RapidJson_FOUND            - True if RapidJSON has been found and can be used
#
# and the following imported targets::
#
#   RapidJson::RapidJson  - The RapidJSON library

cmake_minimum_required(VERSION 3.2)

find_path(RapidJson_INCLUDE_DIR
    NAMES "rapidjson/rapidjson.h"
    HINTS ${RAPIDJSON_INCLUDEDIR} "${PROJECT_SOURCE_DIR}/thirdparty/rapidjson/include"
)

# Set Rapidjson_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RapidJson
    FOUND_VAR RapidJson_FOUND
    REQUIRED_VARS RapidJson_INCLUDE_DIR
)
if(RapidJson_FOUND)
    set(RapidJson_INCLUDE_DIRS ${RapidJson_INCLUDE_DIR})
endif()

mark_as_advanced(
    RapidJson_INCLUDE_DIRS
)
