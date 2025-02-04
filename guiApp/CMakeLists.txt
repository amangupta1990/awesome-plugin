
cmake_minimum_required(VERSION 3.22)

project(GuiApp VERSION 0.1.0)

juce_add_gui_app(${PROJECT_NAME}
    PRODUCT_NAME "one plugin to rule them all"
    COMPANY_NAME "Aman_withaHorn"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/MainComponent.h
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        headers/myPlugin
)

juce_generate_juce_header(${PROJECT_NAME})

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
)