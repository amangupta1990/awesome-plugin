#pragma once

#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h> // Include the necessary header

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              public juce::Slider::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor &processorRef;

    juce::Slider midiVolumeSlider; // Slider to control MIDI volume

    void sliderValueChanged(juce::Slider *slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};