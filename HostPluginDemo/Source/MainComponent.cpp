#include "MainComponent.h"
#include "MenuBarComponent.h"
#include "PluginEditorComponent.h"
#include <iostream>

extern juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

MainComponent::MainComponent()
    : Thread("PluginScannerThread")
{
    std::cout << "MainComponent Constructor" << std::endl;

    setSize(juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea.getWidth(),
            juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea.getHeight()); // Fit the entire screen

    // Add the ComboBox to the component
    addAndMakeVisible(vstComboBox);
    vstComboBox.addListener(this);

    // Initialize the audio device manager
    if (juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager.getAudioDeviceSetup(setup);
        setup.useDefaultInputChannels = true;
        setup.useDefaultOutputChannels = true;
        deviceManager.setAudioDeviceSetup(setup, true);

        juce::String error = deviceManager.initialiseWithDefaultDevices(2, 2);
        if (!error.isEmpty())
        {
            std::cerr << "Error initializing audio device: " << error << std::endl;
        }
        else
        {
            auto* currentAudioDevice = deviceManager.getCurrentAudioDevice();
            if (currentAudioDevice != nullptr)
            {
                std::cout << "Current audio device: " << currentAudioDevice->getName() << std::endl;
            }
            else
            {
                std::cerr << "No audio device found" << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "Audio recording permission not granted" << std::endl;
    }

    deviceManager.addAudioCallback(&audioProcessorPlayer);
    audioProcessorPlayer.setProcessor(&audioGraph);

    // Add a listener to detect changes in audio devices
    deviceManager.addChangeListener(this);

    // Initialize and add the MenuBarComponent to the component
    menuBarComponent = std::make_unique<::MenuBarComponent>(&commandManager, deviceManager);
    menuBarComponent->setMuteCallback([this](bool muted) { handleMuteEvent(muted); });
    menuBarComponent->setBypassCallback([this](bool bypassed) { handleBypassEvent(bypassed); });
    addAndMakeVisible(menuBarComponent.get());
    menuBarComponent->setBounds(0, 20, getWidth(), 60); // Shift the menu bar just below the status bar

    // Add the plugin viewport and container
    addAndMakeVisible(pluginViewport);
    pluginViewport.setViewedComponent(&pluginContainer, false);
    pluginViewport.setScrollBarsShown(false, false);

    // Manually add the plugin formats
    formatManager.addFormat(new juce::AudioUnitPluginFormat());
    std::cout << "Format Manager Initialized with VST3 and AudioUnit formats" << std::endl;

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

    // Add input and output nodes to the audio graph
    inputNode = audioGraph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
    outputNode = audioGraph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

    if (inputNode != nullptr && outputNode != nullptr)
    {
        std::cout << "Input and output nodes added to the audio graph" << std::endl;
        audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
        std::cout << "Connection between input and output nodes established" << std::endl;
    }
    else
    {
        std::cout << "Failed to add input or output nodes to the audio graph" << std::endl;
    }

    resized();
    startThread();

    // Add menu bar
    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);
    addAndMakeVisible(menuBarComponent->menuBar);
    menuBarComponent->menuBar.setModel(this);
    menuBarComponent->menuBar.setBounds(0, 20, getWidth(), 20); // Shift the command manager just below the status bar
}

MainComponent::~MainComponent()
{
    std::cout << "MainComponent Destructor" << std::endl;
    stopThread(2000); // Stop the thread with a timeout of 2 seconds
    deviceManager.removeAudioCallback(&audioProcessorPlayer);
    deviceManager.removeChangeListener(this);
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black); // Set slate grey background color
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
}

void MainComponent::resized()
{
    const int statusBarHeight = 20; // Adjust this value based on the actual status bar height


    vstComboBox.setBounds(10, statusBarHeight + 90, getWidth() - 20, 30); // Adjusted to ensure it stays below the menu bar
    pluginViewport.setBounds(10, statusBarHeight + 130, getWidth() - 20, getHeight() - (statusBarHeight + 140)); // Adjusted to ensure it stays below the menu bar

    int totalWidth = 0;
    for (auto *editor : pluginEditorComponents)
    {
        totalWidth += editor->getWidth() + 10; // Calculate total width needed for all editors
    }

    int x = (getWidth() - totalWidth) / 2; // Start from the center horizontally
    x = std::max(x, 10); // Ensure it does not go out of bounds

    for (auto *editor : pluginEditorComponents)
    {
        int editorHeight = editor->getHeight();
        int y = (getHeight() - editorHeight) / 2; // Center align vertically

        editor->setBounds(x, y, editor->getWidth(), editorHeight);
        x += editor->getWidth() + 20; // Add uniform spacing
    }

    pluginContainer.setSize(x, getHeight() - (statusBarHeight + 150)); // Update the container size
    pluginViewport.setViewPosition(0, 0); // Ensure the viewport starts at the top
}

// ... rest of the code ...

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
        std::cout << "Plugin format name: " << pluginDescription.pluginFormatName << std::endl;
        std::cout << "Plugin file or identifier: " << pluginDescription.fileOrIdentifier << std::endl;

        // Get the format based on the plugin description
        juce::AudioPluginFormat *format = nullptr;
        for (int i = 0; i < formatManager.getNumFormats(); ++i)
        {
            auto *currentFormat = formatManager.getFormat(i);
            std::cout << "Checking format: " << currentFormat->getName() << std::endl;
            if (currentFormat->getName() == pluginDescription.pluginFormatName)
            {
                format = currentFormat;
                break;
            }
        }

        if (format != nullptr)
        {
            std::cout << "Format found: " << format->getName() << std::endl;
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
                        auto* editorComponent = new PluginEditorComponent(
                            std::unique_ptr<juce::AudioProcessorEditor>(editor),
                            [this, nodeId] { removePluginFromGraph(nodeId); },
                            [this] { resized(); } // Pass a lambda that calls the resized method
                        );
                        editorComponent->setNodeID(nodeId); // Associate the editor component with the node ID
                        pluginEditorComponents.add(editorComponent);
                        pluginContainer.addAndMakeVisible(editorComponent);
                        resized(); // Update layout
                    }

                    // Connect the new plugin in the chain
                    if (pluginEditorComponents.size() == 1)
                    {
                        // First plugin, connect input node to this plugin
                        audioGraph.removeConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
                        audioGraph.addConnection({{inputNode->nodeID, 0}, {nodeId, 0}});
                    }
                    else
                    {
                        // Connect the previous plugin to this plugin
                        auto previousPluginNodeID = pluginEditorComponents[pluginEditorComponents.size() - 2]->getNodeID();
                        audioGraph.addConnection({{previousPluginNodeID, 0}, {nodeId, 0}});
                    }

                    // Connect this plugin to the output node
                    audioGraph.addConnection({{nodeId, 0}, {outputNode->nodeID, 0}});
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
    // Find the index of the plugin to be removed
    int indexToRemove = -1;
    for (int i = 0; i < pluginEditorComponents.size(); ++i)
    {
        if (pluginEditorComponents[i]->getNodeID() == nodeId)
        {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove != -1)
    {
        // Remove the plugin from the graph
        audioGraph.removeNode(nodeId);

        // Remove the plugin editor component
        pluginEditorComponents.remove(indexToRemove);

        // Reconnect the remaining plugins
        if (pluginEditorComponents.isEmpty())
        {
            // No plugins left, connect input node to output node
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
        }
        else
        {
            // Reconnect the remaining plugins
            for (int i = 0; i < pluginEditorComponents.size(); ++i)
            {
                auto currentPluginNodeID = pluginEditorComponents[i]->getNodeID();
                if (i == 0)
                {
                    // First plugin, connect input node to this plugin
                    audioGraph.addConnection({{inputNode->nodeID, 0}, {currentPluginNodeID, 0}});
                }
                else
                {
                    // Connect the previous plugin to this plugin
                    auto previousPluginNodeID = pluginEditorComponents[i - 1]->getNodeID();
                    audioGraph.addConnection({{previousPluginNodeID, 0}, {currentPluginNodeID, 0}});
                }

                // Connect this plugin to the output node
                audioGraph.addConnection({{currentPluginNodeID, 0}, {outputNode->nodeID, 0}});
            }
        }

        resized(); // Update layout
    }
}

void MainComponent::run()
{
    scanForPlugins(pluginMap);
    // populate the combo box with the plugin names
    for (const auto &entry : pluginMap)
    {
        vstComboBox.addItem(entry.name, vstComboBox.getNumItems() + 1);
    }
}

// Menu bar related methods
juce::StringArray MainComponent::getMenuBarNames()
{
    return {"File", "Edit", "Options"};
}

juce::PopupMenu MainComponent::getMenuForIndex(int /*menuIndex*/, const juce::String &menuName)
{
    juce::PopupMenu menu;

    if (menuName == "Options")
    {
        juce::StringArray inputDevices = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(true);
        juce::StringArray outputDevices = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(false);

        menu.addSectionHeader("Input Devices");
        auto currentInputDevice = deviceManager.getCurrentAudioDevice()->getName();
        for (int i = 0; i < inputDevices.size(); ++i)
        {
            menu.addItem(1000 + i, inputDevices[i], true, currentInputDevice == inputDevices[i]);
        }

        menu.addSectionHeader("Output Devices");
        auto currentOutputDevice = deviceManager.getCurrentAudioDevice()->getName();
        for (int i = 0; i < outputDevices.size(); ++i)
        {
            menu.addItem(2000 + i, outputDevices[i], true, currentOutputDevice == outputDevices[i]);
        }
    }

    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/)
{
    if (menuItemID >= 1000 && menuItemID < 2000)
    {
        int inputIndex = menuItemID - 1000;
        auto inputDeviceName = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(true)[inputIndex];
        auto setup = deviceManager.getAudioDeviceSetup();
        setup.inputDeviceName = inputDeviceName;
        juce::String error = deviceManager.setAudioDeviceSetup(setup, true);
        if (!error.isEmpty())
        {
            std::cerr << "Error setting input device: " << error << std::endl;
        }
        else
        {
            std::cout << "Input device set to: " << inputDeviceName << std::endl;
        }
        menuBarComponent->setInputSource(inputDeviceName); // Update input source
    }
    else if (menuItemID >= 2000)
    {
        int outputIndex = menuItemID - 2000;
        auto outputDeviceName = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(false)[outputIndex];
        auto setup = deviceManager.getAudioDeviceSetup();
        setup.outputDeviceName = outputDeviceName;
        juce::String error = deviceManager.setAudioDeviceSetup(setup, true);
        if (!error.isEmpty())
        {
            std::cerr << "Error setting output device: " << error << std::endl;
        }
        else
        {
            std::cout << "Output device set to: " << outputDeviceName << std::endl;
        }
    }
}

void MainComponent::handleMuteEvent(bool muted)
{
    std::cout << "Mute event: " << muted << std::endl;
    if (muted)
    {
        // Remove the connection to the output node
        audioGraph.removeConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
        for (auto *editor : pluginEditorComponents)
        {
            audioGraph.removeConnection({{editor->getNodeID(), 0}, {outputNode->nodeID, 0}});
        }
    }
    else
    {
        // Reconnect the appropriate node to the output node based on the bypass state
        if (menuBarComponent->isBypassed())
        {
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
        }
        else if (!pluginEditorComponents.isEmpty())
        {
            auto lastPluginNodeID = pluginEditorComponents.getLast()->getNodeID();
            audioGraph.addConnection({{lastPluginNodeID, 0}, {outputNode->nodeID, 0}});
        }
        else
        {
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
        }
    }
}

void MainComponent::handleBypassEvent(bool bypassed)
{
    std::cout << "Bypass event: " << bypassed << std::endl;

    if (bypassed)
    {
        // Remove all connections from the input node to the plugins
        for (auto *editor : pluginEditorComponents)
        {
            audioGraph.removeConnection({{inputNode->nodeID, 0}, {editor->getNodeID(), 0}});
            audioGraph.removeConnection({{editor->getNodeID(), 0}, {outputNode->nodeID, 0}});
        }

        // Connect the input node directly to the output node
        audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
    }
    else
    {
        // Remove the direct connection from the input node to the output node
        audioGraph.removeConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});

        // Reconnect the input node to the first plugin in the chain
        if (!pluginEditorComponents.isEmpty())
        {
            auto firstPluginNodeID = pluginEditorComponents.getFirst()->getNodeID();
            audioGraph.addConnection({{inputNode->nodeID, 0}, {firstPluginNodeID, 0}});

            // Connect each plugin to the next one in the chain
            for (int i = 1; i < pluginEditorComponents.size(); ++i)
            {
                auto previousPluginNodeID = pluginEditorComponents[i - 1]->getNodeID();
                auto currentPluginNodeID = pluginEditorComponents[i]->getNodeID();
                audioGraph.addConnection({{previousPluginNodeID, 0}, {currentPluginNodeID, 0}});
            }

            // Connect the last plugin to the output node
            auto lastPluginNodeID = pluginEditorComponents.getLast()->getNodeID();
            audioGraph.addConnection({{lastPluginNodeID, 0}, {outputNode->nodeID, 0}});
        }
        else
        {
            // If no plugins are present, connect the input node to the output node
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
        }
    }

    // Handle mute state after bypass state change
    handleMuteEvent(menuBarComponent->isMuted());
}

// AudioAppComponent methods
void MainComponent::prepareToPlay(int /*samplesPerBlockExpected*/, double /*sampleRate*/)
{
    // Prepare to play audio
}

void MainComponent::releaseResources()
{
    // Release audio resources
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    // Capture the input signal level
    float maxLevel = 0.0f;
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto *channelData = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            maxLevel = std::max(maxLevel, std::abs(channelData[sample]));
        }
    }

    if (menuBarComponent->isBypassed())
    {
        // If bypassed, read from the input node directly
        juce::AudioBuffer<float> inputBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
        juce::MidiBuffer midiBuffer; // Create a MidiBuffer object
        inputNode->getProcessor()->processBlock(inputBuffer, midiBuffer);
        maxLevel = inputBuffer.getMagnitude(0, inputBuffer.getNumSamples());
    }

    

    // If muted, clear the buffer
    if (menuBarComponent->isMuted())
    {
        bufferToFill.clearActiveBufferRegion();
    }
}
// ApplicationCommandTarget methods
juce::ApplicationCommandTarget *MainComponent::getNextCommandTarget()
{
    return nullptr;
}

void MainComponent::getAllCommands(juce::Array<juce::CommandID> & /*commands*/)
{
    // Get all commands
}

void MainComponent::getCommandInfo(juce::CommandID /*commandID*/, juce::ApplicationCommandInfo & /*result*/)
{
    // Get command info
}

bool MainComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo & /*info*/)
{
    return false;
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &deviceManager)
    {
        // Update the menu bar with the new list of devices
        menuBarComponent->menuBar.repaint();
        std::cout << "Audio devices changed. Available devices:" << std::endl;
        auto inputDevices = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(true);
        auto outputDevices = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(false);
        for (const auto& device : inputDevices)
        {
            std::cout << "Input device: " << device << std::endl;
        }
        for (const auto& device : outputDevices)
        {
            std::cout << "Output device: " << device << std::endl;
        }
    }
}
