cmake_minimum_required(VERSION 3.22)

project(AudioPluginStarter)

set(CMAKE_CXX_STANDARD 23)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Ensure CPM.cmake exists or fetch it
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpm.cmake")
    message(STATUS "Downloading CPM.cmake...")
    file(DOWNLOAD 
         https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake 
         ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpm.cmake 
         STATUS download_status)

    list(GET download_status 0 status_code)
    if (NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download CPM.cmake. Please download it manually from: "
                            "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake")
    endif()
endif()

# Include CPM
include(cmake/cpm.cmake)

# Use CPM to add dependencies
CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 8.0.4
    VERSION 8.0.4
    SOURCE_DIR ${LIB_DIR}/juce
)

# # Define your JUCE plugin
# juce_add_plugin(AudioPluginStarter
#     PLUGIN_MANUFACTURER "YourCompany"
#     PLUGIN_CODE "Demo"
#     FORMATS VST3 AU Standalone
#     SOURCES src/PluginProcessor.cpp src/PluginEditor.cpp
# )

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

if (MSVC)
    add_compile_options(/Wall /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(plugin)