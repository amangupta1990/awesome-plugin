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

    // Add the VolumeMeter to the component
    addAndMakeVisible(volumeMeter);

    // Add the plugin viewport and container
    addAndMakeVisible(pluginViewport);
    pluginViewport.setViewedComponent(&pluginContainer, false);

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

    // Add input and output nodes to the audio graph
    auto inputNode = audioGraph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
    auto outputNode = audioGraph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

    if (inputNode != nullptr && outputNode != nullptr)
    {
        std::cout << "Input and output nodes added to the audio graph" << std::endl;
        audioGraph.addConnection({{inputNode->nodeID, juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode}, {outputNode->nodeID, juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode}});
        std::cout << "Connection between input and output nodes established" << std::endl;
    }
    else
    {
        std::cout << "Failed to add input or output nodes to the audio graph" << std::endl;
    }

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
    volumeMeter.setBounds(10, 50, getWidth() - 20, 30);
    pluginViewport.setBounds(10, 90, getWidth() - 20, getHeight() - 100);

    int x = 0;
    for (auto *editor : pluginEditorComponents)
    {
        editor->setBounds(x, 0, 300, 200); // Set wider dimensions for the plugin editors
        x += 310;                          // Add some spacing between editors
    }

    pluginContainer.setSize(x, 200); // Update the container size
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

        // Add section heading for the format
        vstComboBox.addSectionHeading(format->getName());

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
            pluginDescription.name = pluginDescription.descriptiveName.isNotEmpty() ? pluginDescription.descriptiveName : pluginName;
            pluginDescription.pluginFormatName = format->getName();
            pluginMap.set(pluginName, pluginDescription);

            // Add plugin to ComboBox
            vstComboBox.addItem(pluginDescription.name, vstComboBox.getNumItems() + 1);
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
    if (pluginMap.contains(pluginName))
    {
        const juce::PluginDescription &pluginDescription = pluginMap[pluginName];
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

                    // Create and add the plugin editor component
                    auto* pluginInstance = audioGraph.getNodeForId(nodeId)->getProcessor();
                    auto editor = pluginInstance->createEditorIfNeeded();
                    if (editor != nullptr)
                    {
                        auto* editorComponent = new PluginEditorComponent(std::unique_ptr<juce::AudioProcessorEditor>(editor), [this, nodeId] { removePluginFromGraph(nodeId); });
                        pluginEditorComponents.add(editorComponent);
                        pluginContainer.addAndMakeVisible(editorComponent);
                        resized(); // Update layout
                    }
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

void MainComponent::removePluginFromGraph(juce::AudioProcessorGraph::NodeID nodeId)
{
    audioGraph.removeNode(nodeId);
    for (int i = 0; i < pluginEditorComponents.size(); ++i)
    {
        if (pluginEditorComponents[i]->getBounds().contains(pluginEditorComponents[i]->getBounds().getCentre()))
        {
            pluginEditorComponents.remove(i);
            break;
        }
    }
    resized(); // Update layout
}

void MainComponent::run()
{
    scanForPlugins();
}