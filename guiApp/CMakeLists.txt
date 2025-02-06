cmake_minimum_required(VERSION 3.22)

project(GuiApp VERSION 0.1.0)

# Set the default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_SYSROOT iphoneos)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0") # Set your minimum iOS version

# Add debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

juce_add_gui_app(${PROJECT_NAME}
    PRODUCT_NAME "one plugin to rule them all"
    COMPANY_NAME "Aman_withaHorn"
    BUNDLE_ID "com.yourcompany.awesomeplugin"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
)

target_sources(${PROJECT_NAME}
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/MainComponent.h
        Source/FileAccessHelper.mm
        Source/VolumeMeter.h
        Source/PluginEditorComponent.h
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

juce_generate_juce_header(${PROJECT_NAME})

# Enable JUCE plugin host flags
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        JUCE_PLUGINHOST_VST3=1
        JUCE_PLUGINHOST_AU=1
)

# Link JUCE modules
target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_audio_utils
)