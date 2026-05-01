# Copyright (c) 2026 VillageSQL Contributors
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

cmake_minimum_required(VERSION 3.16)
project(vsql_httpfs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Extension name
set(EXTENSION_NAME "vsql_httpfs")

# Find VillageSQL SDK
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(VillageSQL REQUIRED)

message(STATUS "VillageSQL SDK found at: ${VillageSQL_PREFIX}")
message(STATUS "VillageSQL include dir: ${VillageSQL_INCLUDE_DIR}")

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# Create the httpfs shared library
add_library(httpfs SHARED
    src/httpfs.cc
    src/http_client.cc
    src/s3_client.cc
)

# Include directories
target_include_directories(httpfs PRIVATE
    ${VillageSQLExtensionFramework_INCLUDE_DIR}
    src/include
    ${OPENSSL_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(httpfs PRIVATE
    ${OPENSSL_LIBRARIES}
    ${CURL_LIBRARIES}
)

# Create the VEB package
VEF_CREATE_VEB(
    NAME ${EXTENSION_NAME}
    LIBRARY_TARGET httpfs
    MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json
)
