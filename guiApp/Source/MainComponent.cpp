#include "MainComponent.h"
#include <iostream>

MainComponent::MainComponent()
    : Thread("PluginScannerThread")
{
    std::cout << "MainComponent Constructor" << std::endl;

    setSize(800, 600);

    // Add the ComboBox to the component
    addAndMakeVisible(vstComboBox);
    vstComboBox.addListener(this);

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

    // Initialize the audio device manager
    deviceManager.initialiseWithDefaultDevices(2, 2);
    deviceManager.addAudioCallback(&audioProcessorPlayer);
    audioProcessorPlayer.setProcessor(&audioGraph);

    // Start the plugin scanning thread
    startThread();
}

MainComponent::~MainComponent()
{
    std::cout << "MainComponent Destructor" << std::endl;
    stopThread(2000); // Stop the thread with a timeout of 2 seconds
    deviceManager.removeAudioCallback(&audioProcessorPlayer);
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

    int labelY = 50;
    for (auto *label : pluginLabels)
    {
        label->setBounds(10, labelY, getWidth() - 20, 30);
        labelY += 40;
    }
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

            // Add plugin to the hash map
            juce::PluginDescription pluginDescription;
            pluginDescription.fileOrIdentifier = pluginPath;
            pluginDescription.name = pluginName;
            pluginDescription.pluginFormatName = format->getName();
            pluginMap[pluginName] = pluginDescription;

            vstComboBox.addItem(pluginName, vstComboBox.getNumItems() + 1);
        }
        std::cout << "Finished processing format: " << format->getName() << std::endl;
    }

    std::cout << "Finished scanning for plugins" << std::endl;
}

void MainComponent::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    std::cout << "ComboBox changed" << std::endl;
    if (comboBoxThatHasChanged == &vstComboBox)
    {
        juce::String selectedPlugin = vstComboBox.getText();
        std::cout << "Selected plugin: " << selectedPlugin << std::endl;
        addPluginToGraph(selectedPlugin);
    }
}

void MainComponent::addPluginToGraph(const juce::String &pluginName)
{
    std::cout << "Adding plugin to graph: " << pluginName << std::endl;
    auto it = pluginMap.find(pluginName);
    if (it != pluginMap.end())
    {
        const juce::PluginDescription &pluginDescription = it->second;
        std::cout << "Plugin description found for: " << pluginName << std::endl;

        // Get the format based on the plugin description
        juce::AudioPluginFormat *format = nullptr;
        for (int i = 0; i < formatManager.getNumFormats(); ++i)
        {
            auto *currentFormat = formatManager.getFormat(i);
            if (currentFormat->getName() == pluginDescription.pluginFormatName)
            {
                format = currentFormat;
                break;
            }
        }

        if (format != nullptr)
        {
            format->createPluginInstanceAsync(pluginDescription, 44100.0, 512, [this, pluginName](std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String &error)
                                              {
                if (instance != nullptr)
                {
                    auto nodeId = audioGraph.addNode(std::move(instance))->nodeID;
                    std::cout << "Added plugin to graph with node ID: " << static_cast<int>(nodeId.uid) << std::endl;

                    // Update UI to show the added plugin
                    juce::Label* pluginLabel = new juce::Label();
                    pluginLabel->setText(pluginName, juce::dontSendNotification);
                    addAndMakeVisible(pluginLabel);
                    pluginLabels.add(pluginLabel);
                    resized(); // Update layout
                }
                else
                {
                    std::cout << "Failed to create plugin instance: " << error << std::endl;
                } });
        }
        else
        {
            std::cout << "No format found for plugin description: " << pluginDescription.name << std::endl;
        }
    }
    else
    {
        std::cout << "No plugin description found for: " << pluginName << std::endl;
    }
}
void MainComponent::run()
{
    scanForPlugins();
}