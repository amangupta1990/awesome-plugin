#pragma once

#include <JuceHeader.h>
#include "VolumeMeter.h"
#include "PluginEditorComponent.h"

class MainComponent : public juce::Component,
                      public juce::ComboBox::Listener,
                      public juce::Thread
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void run() override;

private:
    void scanForPlugins();
    void addPluginToGraph(const juce::String &pluginName);
    void removePluginFromGraph(juce::AudioProcessorGraph::NodeID nodeId);

    juce::ComboBox vstComboBox;
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer audioProcessorPlayer;
    juce::AudioProcessorGraph audioGraph;
    juce::KnownPluginList pluginList;
    juce::AudioPluginFormatManager formatManager;
    juce::HashMap<juce::String, juce::PluginDescription> pluginMap;
    juce::OwnedArray<juce::Label> pluginLabels;
    VolumeMeter volumeMeter;
    juce::OwnedArray<PluginEditorComponent> pluginEditorComponents; // Store multiple plugin editors
    juce::Viewport pluginViewport;                                  // Add a viewport for horizontal scrolling
    juce::Component pluginContainer;                                // Container for plugin editors

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};