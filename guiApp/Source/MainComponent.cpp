#include "MainComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>

MainComponent::MainComponent()
{
    DBG("MainComponent Constructor");

    setSize(800, 600);

    // Add the ComboBox to the component
    addAndMakeVisible(vstComboBox);
    vstComboBox.onChange = [this]
    { vstSelected(); };

    // Initialize the format manager and add the default formats (including VST)
    formatManager.addDefaultFormats();
    DBG("Format Manager Initialized");

    // Populate the ComboBox with VST plugins
    populateVSTComboBox();
    DBG("VST ComboBox Populated");
}

MainComponent::~MainComponent()
{
    DBG("MainComponent Destructor");
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::black);
    g.setFont(20.0f);
    g.drawText("Hello, JUCE!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // Set the bounds for the ComboBox
    vstComboBox.setBounds(10, 10, getWidth() - 20, 30);
}

void MainComponent::populateVSTComboBox()
{
    DBG("Populating VST ComboBox");

    // Scan for VST plugins and add them to the ComboBox
    juce::FileSearchPath searchPath;
    searchPath.add(juce::File("/Library/Audio/Plug-Ins/VST").getFullPathName());
    searchPath.add(juce::File("/Library/Audio/Plug-Ins/VST3").getFullPathName());
    searchPath.add(juce::File("~/Library/Audio/Plug-Ins/VST").getFullPathName());
    searchPath.add(juce::File("~/Library/Audio/Plug-Ins/VST3").getFullPathName());
    searchPath.add(juce::File("/Users/amangupta/projects/juce_cmake/build/plugin/AudioPlugin_artefacts/Release/VST3").getFullPathName());
    DBG("Search Path: " + searchPath.toString());

    if (!juce::File("/Library/Audio/Plug-Ins/VST").exists() &&
        !juce::File("/Library/Audio/Plug-Ins/VST3").exists() &&
        !juce::File("~/Library/Audio/Plug-Ins/VST").exists() &&
        !juce::File("~/Library/Audio/Plug-Ins/VST3").exists() &&
        !juce::File("/Users/amangupta/projects/juce_cmake/build/plugin/AudioPlugin_artefacts/Release/VST3").exists())
    {
        DBG("Default VST directories do not exist or cannot be accessed");
        return;
    }

    juce::File deadMansPedalFile; // You can specify a file path if needed
    DBG("Dead Man's Pedal File: " + deadMansPedalFile.getFullPathName());

    auto *format = formatManager.getFormat(0);
    if (format == nullptr)
    {
        DBG("No format found");
        return;
    }
    DBG("Format found: " + format->getName());

    juce::PluginDirectoryScanner scanner(pluginList, *format, searchPath, true, deadMansPedalFile);
    DBG("Plugin Directory Scanner Initialized");

    juce::String pluginName;
    while (scanner.scanNextFile(true, pluginName))
    {
        DBG("Scanning plugin: " + pluginName);
        auto pluginDescription = pluginList.getTypeForFile(pluginName);
        if (pluginDescription != nullptr)
        {
            vstComboBox.addItem(pluginDescription->name, vstComboBox.getNumItems() + 1);
            DBG("Added plugin: " + pluginDescription->name);
        }
        else
        {
            DBG("No plugin description found for: " + pluginName);
        }
    }

    DBG("VST ComboBox Population Complete");
}

void MainComponent::vstSelected()
{
    // Handle VST selection
    auto selectedVST = vstComboBox.getText();
    DBG("Selected VST: " + selectedVST);
}