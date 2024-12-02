cmake_minimum_required(VERSION 3.22)

project(AudioPlugin VERSION 0.1.0)

juce_add_plugin(${PROJECT_NAME}
PLUGIN_MANUFACTURER Aman_withaHorn
IS_SYNTH FALSE
NEEDS_MIDI_INPUT FALSE
NEEDS_MIDI_OUTPUT FALSE
PLUGIN_MANUFACTURER_CODE AmwH
PLUGIN_CODE AmwH
FORMATS VST3 Standalone
PRODUCT_NAME "one plugin to rule them all"
COMPANY_NAME "Aman_withaHorn"
)

target_sources(${PROJECT_NAME}
    PUBLIC
        Source/PluginEditor.cpp
        Source/PluginProcessor.cpp
)

target_include_directories(AudioPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/headers/myPlugin
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        juce::juce_gui_basics
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
)