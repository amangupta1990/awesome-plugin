#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::Component,
                      private juce::ComboBox::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    void scanForPlugins();
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void addPluginToGraph(const juce::String &pluginName);

    juce::ComboBox vstComboBox;
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList pluginList;
    juce::AudioProcessorGraph audioGraph;
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer audioProcessorPlayer;
    juce::OwnedArray<juce::Label> pluginLabels; // Declare pluginLabels

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};