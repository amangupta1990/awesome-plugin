#include "MainComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <iostream>

MainComponent::MainComponent()
{
    std::cout << "MainComponent Constructor" << std::endl;

    setSize(800, 600);

    // Add the ComboBox to the component
    addAndMakeVisible(vstComboBox);

    // Manually add the plugin formats
    formatManager.addFormat(new juce::VST3PluginFormat());
    formatManager.addFormat(new juce::AudioUnitPluginFormat());
    std::cout << "Format Manager Initialized with VST, VST3, and AudioUnit formats" << std::endl;

    // Check if formats are added
    if (formatManager.getNumFormats() == 0)
    {
        std::cout << "No formats added to the format manager" << std::endl;
    }
    else
    {
        for (int i = 0; i < formatManager.getNumFormats(); ++i)
        {
            std::cout << "Format " << i << ": " << formatManager.getFormat(i)->getName() << std::endl;
        }
    }

    // Scan for plugins
    scanForPlugins();
    std::cout << "Plugins Scanned" << std::endl;
}

MainComponent::~MainComponent()
{
    std::cout << "MainComponent Destructor" << std::endl;
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::black);
    g.setFont(20.0f);
    g.drawText("Plugin Host", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    vstComboBox.setBounds(10, 10, getWidth() - 20, 30);
}

void MainComponent::scanForPlugins()
{
    std::cout << "Scanning for plugins" << std::endl;

    juce::FileSearchPath searchPath;
    searchPath.add(juce::File("/Library/Audio/Plug-Ins/VST").getFullPathName());
    searchPath.add(juce::File("/Library/Audio/Plug-Ins/VST3").getFullPathName());
    searchPath.add(juce::File("~/Library/Audio/Plug-Ins/VST").getFullPathName());
    searchPath.add(juce::File("~/Library/Audio/Plug-Ins/VST3").getFullPathName());

    std::cout << "Search paths added: " << searchPath.toString() << std::endl;

    // Use the format manager to get the formats
    for (int i = 0; i < formatManager.getNumFormats(); ++i)
    {
        auto *format = formatManager.getFormat(i);
        std::cout << "Searching for plugins with format: " << format->getName() << std::endl;

        juce::StringArray foundPlugins = format->searchPathsForPlugins(searchPath, true, false);
        std::cout << "Found " << foundPlugins.size() << " plugins for format: " << format->getName() << std::endl;

        if (foundPlugins.size() == 0)
        {
            std::cout << "No plugins found for format: " << format->getName() << std::endl;
        }

        for (int j = 0; j < foundPlugins.size(); ++j)
        {
            juce::String pluginPath = foundPlugins[j];
            std::cout << "Found plugin at path: " << pluginPath << std::endl;
            juce::File pluginFile(pluginPath);
            juce::String pluginName = pluginFile.getFileNameWithoutExtension();
            std::cout << "Adding plugin: " << pluginName << std::endl;
            std::cout << "plugin file: " << pluginFile.getFullPathName() << std::endl;
            vstComboBox.addItem(pluginName, vstComboBox.getNumItems() + 1);
        }
        std::cout << "Finished processing format: " << format->getName() << std::endl;
    }

    std::cout << "Finished scanning for plugins" << std::endl;
}