# Find VillageSQL SDK
# Sets variables: VillageSQL_FOUND, VillageSQL_PREFIX, VillageSQL_INCLUDE_DIR, VillageSQLExtensionFramework_INCLUDE_DIR

if(DEFINED VillageSQL_BUILD_DIR)
    set(VillageSQL_PREFIX ${VillageSQL_BUILD_DIR})
else()
    # Try to find it in common locations
    find_path(VillageSQL_PREFIX
        NAMES include/villagesql/extension.h
        PATHS
            $ENV{HOME}/build/villagesql
            $ENV{HOME}/.local
            /usr/local
            /opt/villagesql
    )
endif()

if(NOT VillageSQL_PREFIX)
    message(FATAL_ERROR "VillageSQL build directory not found. Set VillageSQL_BUILD_DIR cmake variable.")
endif()

set(VillageSQL_INCLUDE_DIR ${VillageSQL_PREFIX}/include)
set(VillageSQLExtensionFramework_INCLUDE_DIR ${VillageSQL_PREFIX}/include)

if(EXISTS ${VillageSQL_INCLUDE_DIR}/villagesql/extension.h)
    set(VillageSQL_FOUND TRUE)
    message(STATUS "Found VillageSQL: ${VillageSQL_PREFIX}")
else()
    message(FATAL_ERROR "VillageSQL extension headers not found at ${VillageSQL_INCLUDE_DIR}")
endif()

# Define VEF_CREATE_VEB macro
function(VEF_CREATE_VEB)
    set(oneValueArgs NAME LIBRARY_TARGET MANIFEST)
    cmake_parse_arguments(VEB "" "${oneValueArgs}" "" ${ARGN})
    
    # Get the library path
    get_target_property(LIBRARY_PATH ${VEB_LIBRARY_TARGET} BINARY_DIR)
    
    # Create VEB (extension package)
    add_custom_target(${VEB_NAME}_veb ALL
        DEPENDS ${VEB_LIBRARY_TARGET}
        COMMAND ${CMAKE_COMMAND} -E echo "Created ${VEB_NAME} extension package"
    )
    
    set_target_properties(${VEB_LIBRARY_TARGET} PROPERTIES PREFIX "")
    set_target_properties(${VEB_LIBRARY_TARGET} PROPERTIES SUFFIX ".veb")
endfunction()
