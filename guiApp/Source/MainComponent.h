#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <unordered_map>

class MainComponent : public juce::Component,
                      private juce::ComboBox::Listener,
                      private juce::Thread
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void scanForPlugins();
    void addPluginToGraph(const juce::String &pluginName);
    void run() override;

    juce::ComboBox vstComboBox;
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer audioProcessorPlayer;
    juce::AudioProcessorGraph audioGraph;
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList pluginList;
    juce::OwnedArray<juce::Label> pluginLabels;

    std::unordered_map<juce::String, juce::PluginDescription> pluginMap; // Hash map to store plugin descriptions

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};