#pragma once

#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h> // Add this line

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    void populateVSTComboBox();
    void vstSelected();

    juce::ComboBox vstComboBox;
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList pluginList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};