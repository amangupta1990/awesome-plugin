#pragma once

#include <JuceHeader.h>
#include "PluginEditorComponent.h"
#include "MenuBarComponent.h"
#include "SelectPluginDialog.h"
#include "PluginDialogWindow.h"



void scanForPlugins(juce::HashMap<juce::String, juce::PluginDescription>& pluginMap);

class MainComponent : public juce::AudioAppComponent,
                      public juce::ChangeListener,
                      public juce::MenuBarModel,
                      public juce::ApplicationCommandTarget,
                      private juce::Thread
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;


    // Menu bar related methods
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String &menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

    // AudioAppComponent methods
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    // ApplicationCommandTarget methods
    juce::ApplicationCommandTarget *getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID> &commands) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo &info) override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void savePluginChain(const juce::File& file);
    void loadPluginChain(const juce::File& file);

private:
    void run() override;
    void addPluginToGraph(const juce::String &pluginName);
    void removePluginFromGraph(juce::AudioProcessorGraph::NodeID nodeId);
    void handleMuteEvent(bool muted);
    void handleBypassEvent(bool bypassed);
    void openPluginDialog();
    


    std::unique_ptr<::MenuBarComponent> menuBarComponent;
    juce::Viewport pluginViewport;
    juce::Component pluginContainer;
    juce::OwnedArray<PluginEditorComponent> pluginEditorComponents;

    juce::AudioPluginFormatManager formatManager;
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer audioProcessorPlayer;
    juce::AudioProcessorGraph audioGraph;
    juce::AudioProcessorGraph::Node::Ptr inputNode; // Declare inputNode
    juce::AudioProcessorGraph::Node::Ptr pluginNode;
    juce::AudioProcessorGraph::Node::Ptr outputNode;
    juce::DrawableButton addPluginButton { "Add", juce::DrawableButton::ButtonStyle::ImageFitted };

    juce::HashMap<juce::String, juce::PluginDescription> pluginMap;
    juce::KnownPluginList pluginList;

    juce::ApplicationCommandManager commandManager;

    std::unique_ptr<SelectPluginDialog> pluginDialog;
    std::unique_ptr<PluginDialogWindow> pluginDialogWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
