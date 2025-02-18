#include "MainComponent.h"
#include "MenuBarComponent.h"
#include "PluginEditorComponent.h"
#include <iostream>

extern juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

MainComponent::MainComponent()
    : Thread("PluginScannerThread"),
      menuBarComponent(std::make_unique<::MenuBarComponent>(&commandManager, deviceManager)) // Initialize menuBarComponent
{
    std::cout << "MainComponent Constructor" << std::endl;

    setSize(juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea.getWidth(),
            juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea.getHeight()); // Fit the entire screen

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
    menuBarComponent->setMuteCallback([this](bool muted) { handleMuteEvent(muted); });
    menuBarComponent->setBypassCallback([this](bool bypassed) { handleBypassEvent(bypassed); });
    addAndMakeVisible(menuBarComponent.get());
    menuBarComponent->setBounds(0, 20, getWidth(), 100); // Increase height to ensure visibility

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
        audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 1}}); // Ensure input is heard on both channels
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

    // Add a '+' button to open the plugin dialog at the end of the viewport
    addPluginButton.onClick = [this] { openPluginDialog(); };
    addPluginButton.setButtonText("+");
    addPluginButton.setSize(50, 50); // Set size for the button

    auto svgData = R"(
<svg width="14" height="14" viewBox="0 0 14 14" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M7 1V13M1 7H13" stroke="white" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
</svg>
    )";
    auto svgDrawable = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(svgData));
    addPluginButton.setImages(svgDrawable.get());

    // Load the saved plugin chain state
    juce::File pluginStateFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("pluginChainState.xml");
    if (pluginStateFile.existsAsFile())
    {
        loadPluginChain(pluginStateFile);
    }
}

MainComponent::~MainComponent()
{
    std::cout << "MainComponent Destructor" << std::endl;

    // Save the plugin chain state before quitting
    juce::File pluginStateFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("pluginChainState.xml");
    savePluginChain(pluginStateFile);

    stopThread(2000); // Stop the thread with a timeout of 2 seconds
    deviceManager.removeAudioCallback(&audioProcessorPlayer);
    deviceManager.removeChangeListener(this);
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0x1E1E1E)); 
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
}

void MainComponent::resized()
{
    const int statusBarHeight = 20; // Adjust this value based on the actual status bar height
    const int menuBarHeight = 160; // Increase the height to ensure visibility of buttons

    menuBarComponent->setBounds(0, 0, getWidth(), menuBarHeight); // Position the menu bar below the status bar

    pluginViewport.setBounds(10, statusBarHeight + menuBarHeight + 10, getWidth() - 20, getHeight() - (statusBarHeight + menuBarHeight + 20)); // Adjusted to ensure it stays below the menu bar

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
        int y = pluginViewport.getHeight() / 2 - editorHeight / 2; // Center the editor vertically

        editor->setBounds(x, y, editor->getWidth(), editorHeight);
        x += editor->getWidth() + 20; // Add uniform spacing
    }

    pluginContainer.setSize(x + 60, getHeight() - (statusBarHeight + menuBarHeight + 20)); // Update the container size
    pluginViewport.setViewPosition(0, 0); // Ensure the viewport starts at the top

    // Add a '+' button to open the plugin dialog at the end of the viewport
    if (pluginEditorComponents.isEmpty())
    {
        addPluginButton.setBounds((getWidth() - addPluginButton.getWidth()) / 2, (getHeight() / 2 - addPluginButton.getHeight() / 2), addPluginButton.getWidth(), addPluginButton.getHeight());
    }
    else
    {
        addPluginButton.setBounds(x, pluginViewport.getHeight() / 2 - addPluginButton.getHeight() / 2, addPluginButton.getWidth(), addPluginButton.getHeight());
    }
    pluginContainer.addAndMakeVisible(addPluginButton);
}

// ... rest of the code ...

void MainComponent::openPluginDialog()
{
    if (pluginDialogWindow == nullptr)
    {
        // Create the dialog window
        pluginDialogWindow = std::make_unique<PluginDialogWindow>(
            "Select Plugin",
            pluginMap,
            [this](const juce::String& pluginName) { addPluginToGraph(pluginName); }
        );

        // Ensure it is visible and on top
        pluginDialogWindow->setAlwaysOnTop(true);
        pluginDialogWindow->toFront(true);
    }
    else
    {
        pluginDialogWindow->setVisible(true); // Reuse existing dialog
        pluginDialogWindow->toFront(true);
    }
}

// Add a member variable to manage the dialog's lifecycle
std::unique_ptr<SelectPluginDialog> pluginDialog;

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
                        audioGraph.removeConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 1}});
                        audioGraph.addConnection({{inputNode->nodeID, 0}, {nodeId, 0}});
                        audioGraph.addConnection({{inputNode->nodeID, 1}, {nodeId, 1}});
                        audioGraph.addConnection({{inputNode->nodeID, 0}, {nodeId, 1}});
                        audioGraph.addConnection({{inputNode->nodeID, 1}, {nodeId, 0}});
                    }
                    else
                    {
                        // Connect the previous plugin to this plugin
                        auto previousPluginNodeID = pluginEditorComponents[pluginEditorComponents.size() - 2]->getNodeID();
                        audioGraph.addConnection({{previousPluginNodeID, 0}, {nodeId, 0}});
                        audioGraph.addConnection({{previousPluginNodeID, 1}, {nodeId, 1}});
                        audioGraph.addConnection({{previousPluginNodeID, 0}, {nodeId, 1}});
                        audioGraph.addConnection({{previousPluginNodeID, 1}, {nodeId, 0}});
                    }

                    // Connect this plugin to the output node
                    audioGraph.addConnection({{nodeId, 0}, {outputNode->nodeID, 0}});
                    audioGraph.addConnection({{nodeId, 0}, {outputNode->nodeID, 1}}); // Ensure plugin is heard on both channels
                    audioGraph.addConnection({{nodeId, 0}, {outputNode->nodeID, 1}});
                    audioGraph.addConnection({{nodeId, 0}, {outputNode->nodeID, 0}});
                }
                else
                {
                    std::cout << "Failed to create plugin instance: " << error << std::endl;
                } });

            // Close the dialog after adding the plugin
            if (pluginDialogWindow != nullptr)
            {
                pluginDialogWindow->setVisible(false);
            }
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
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 1}}); // Ensure input is heard on both channels
            audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 1}});
            audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 0}}); // Ensure input is heard on both channels
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
                    audioGraph.addConnection({{inputNode->nodeID, 1}, {currentPluginNodeID, 1}});
                    audioGraph.addConnection({{inputNode->nodeID, 0}, {currentPluginNodeID, 1}});
                    audioGraph.addConnection({{inputNode->nodeID, 1}, {currentPluginNodeID, 0}});
                }
                else
                {
                    // Connect the previous plugin to this plugin
                    auto previousPluginNodeID = pluginEditorComponents[i - 1]->getNodeID();
                    audioGraph.addConnection({{previousPluginNodeID, 0}, {currentPluginNodeID, 0}});
                    audioGraph.addConnection({{previousPluginNodeID, 1}, {currentPluginNodeID, 1}});
                    audioGraph.addConnection({{previousPluginNodeID, 0}, {currentPluginNodeID, 1}});
                    audioGraph.addConnection({{previousPluginNodeID, 1}, {currentPluginNodeID, 0}});
                }

                // Connect this plugin to the output node
                audioGraph.addConnection({{currentPluginNodeID, 0}, {outputNode->nodeID, 0}});
                audioGraph.addConnection({{currentPluginNodeID, 0}, {outputNode->nodeID, 1}}); // Ensure plugin is heard on both channels
                audioGraph.addConnection({{currentPluginNodeID, 1}, {outputNode->nodeID, 1}});
                audioGraph.addConnection({{currentPluginNodeID, 1}, {outputNode->nodeID, 0}}); // Ensure plugin is heard on both channels
            }
        }

        resized(); // Update layout
    }
}

void MainComponent::run()
{
    scanForPlugins(pluginMap);
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
        audioGraph.removeConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 1}});
        for (auto *editor : pluginEditorComponents)
        {
            audioGraph.removeConnection({{editor->getNodeID(), 0}, {outputNode->nodeID, 0}});
            audioGraph.removeConnection({{editor->getNodeID(), 0}, {outputNode->nodeID, 1}});
            audioGraph.removeConnection({{editor->getNodeID(), 1}, {outputNode->nodeID, 1}});
            audioGraph.removeConnection({{editor->getNodeID(), 1}, {outputNode->nodeID, 0}});
        }
    }
    else
    {
        // Reconnect the appropriate node to the output node based on the bypass state
        if (menuBarComponent->isBypassed())
        {
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 1}});
            audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 1}});
            audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 1}}); // Ensure input is heard on both channels
        }
        else if (!pluginEditorComponents.isEmpty())
        {
            auto lastPluginNodeID = pluginEditorComponents.getLast()->getNodeID();
            audioGraph.addConnection({{lastPluginNodeID, 0}, {outputNode->nodeID, 0}});
            audioGraph.addConnection({{lastPluginNodeID, 0}, {outputNode->nodeID, 1}});
            audioGraph.addConnection({{lastPluginNodeID, 1}, {outputNode->nodeID, 0}});
            audioGraph.addConnection({{lastPluginNodeID, 1}, {outputNode->nodeID, 1}});
        }
        else
        {
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 1}}); // Ensure input is heard on both channels
            audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 0}});
            audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 1}}); // Ensure input is heard on both channels
        }
    }
}
void MainComponent::handleBypassEvent(bool bypassed)
{
    std::cout << "Bypass event: " << bypassed << std::endl;

    if (inputNode == nullptr || outputNode == nullptr)
    {
        std::cerr << "Error: Input or Output node is missing!" << std::endl;
        return;
    }

    // Remove all current connections
    auto connections = audioGraph.getConnections();
    for (const auto& connection : connections)
    {
        audioGraph.removeConnection(connection);
    }

    if (bypassed)
    {
        std::cout << "Bypassing plugins - Routing Dry Input to Stereo Output" << std::endl;

        // Ensure both input channels are routed equally to both output channels
        audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
        audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 1}});
        audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 1}});
        audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 0}});
    }
    else
    {
        std::cout << "Re-enabling plugins - Restoring processing chain" << std::endl;

        if (!pluginEditorComponents.isEmpty())
        {
            auto firstPluginNodeID = pluginEditorComponents.getFirst()->getNodeID();

            // Connect input node to first plugin node
            audioGraph.addConnection({{inputNode->nodeID, 0}, {firstPluginNodeID, 0}});
            audioGraph.addConnection({{inputNode->nodeID, 1}, {firstPluginNodeID, 1}});
            audioGraph.addConnection({{inputNode->nodeID, 0}, {firstPluginNodeID, 1}});
            audioGraph.addConnection({{inputNode->nodeID, 1}, {firstPluginNodeID, 0}});

            // Connect each plugin in sequence
            for (int i = 1; i < pluginEditorComponents.size(); ++i)
            {
                auto prevNodeID = pluginEditorComponents[i - 1]->getNodeID();
                auto currNodeID = pluginEditorComponents[i]->getNodeID();

                audioGraph.addConnection({{prevNodeID, 0}, {currNodeID, 0}});
                audioGraph.addConnection({{prevNodeID, 1}, {currNodeID, 1}});
                audioGraph.addConnection({{prevNodeID, 0}, {currNodeID, 1}});
                audioGraph.addConnection({{prevNodeID, 1}, {currNodeID, 0}});
            }

            // Connect last plugin to both stereo outputs
            auto lastPluginNodeID = pluginEditorComponents.getLast()->getNodeID();
            audioGraph.addConnection({{lastPluginNodeID, 0}, {outputNode->nodeID, 0}});
            audioGraph.addConnection({{lastPluginNodeID, 1}, {outputNode->nodeID, 1}});
            audioGraph.addConnection({{lastPluginNodeID, 0}, {outputNode->nodeID, 1}});
            audioGraph.addConnection({{lastPluginNodeID, 1}, {outputNode->nodeID, 0}});
        }
        else
        {
            std::cout << "No plugins detected - Connecting Input directly to Output" << std::endl;
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
            audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 1}});
            audioGraph.addConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 1}});
            audioGraph.addConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 0}});
        }
    }

    resized();
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
    if (menuBarComponent->isBypassed())
    {
        // Process dry signal
        juce::AudioBuffer<float> inputBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
        juce::MidiBuffer midiBuffer;

        inputNode->getProcessor()->processBlock(inputBuffer, midiBuffer);

        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            float dryLeft = inputBuffer.getSample(0, sample);
            float dryRight = (inputBuffer.getNumChannels() > 1) ? inputBuffer.getSample(1, sample) : dryLeft;

            // Send a fully balanced mix of both inputs to both output channels
            bufferToFill.buffer->setSample(0, sample, 0.5f * (dryLeft + dryRight));
            bufferToFill.buffer->setSample(1, sample, 0.5f * (dryLeft + dryRight));
        }
        return;
    }

    // Process wet (plugin-processed) signal
    juce::AudioBuffer<float> processedBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::MidiBuffer midiBuffer;
    inputNode->getProcessor()->processBlock(processedBuffer, midiBuffer);

    for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        float wetLeft = processedBuffer.getSample(0, sample);
        float wetRight = (processedBuffer.getNumChannels() > 1) ? processedBuffer.getSample(1, sample) : wetLeft;

        // Ensure dry and wet signals are mixed equally across both channels
        bufferToFill.buffer->setSample(0, sample, (wetLeft + wetRight) * 0.5f);
        bufferToFill.buffer->setSample(1, sample, (wetLeft + wetRight) * 0.5f);
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

void MainComponent::savePluginChain(const juce::File& file)
{
    juce::XmlElement xml("PLUGIN_CHAIN");

    for (auto* editor : pluginEditorComponents)
    {
        auto* processor = editor->getAudioProcessor();
        juce::XmlElement* pluginXml = xml.createNewChildElement("PLUGIN");
        pluginXml->setAttribute("id", static_cast<int>(editor->getNodeID().uid));
        pluginXml->setAttribute("name", processor->getName());

        juce::MemoryBlock stateData;
        processor->getStateInformation(stateData);
        std::unique_ptr<juce::XmlElement> stateXml = juce::AudioPluginInstance::getXmlFromBinary(stateData.getData(), stateData.getSize());
        if (stateXml != nullptr)
        {
            pluginXml->addChildElement(stateXml.release());
        }
    }

    xml.writeToFile(file, {});
}

void MainComponent::loadPluginChain(const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(file));

    if (xml == nullptr || !xml->hasTagName("PLUGIN_CHAIN"))
        return;

    // Clear existing plugins
    for (auto* editor : pluginEditorComponents)
    {
        audioGraph.removeNode(editor->getNodeID());
    }
    pluginEditorComponents.clear();

    for (auto* pluginXml : xml->getChildIterator())
    {
        juce::String pluginName = pluginXml->getStringAttribute("name");
        juce::XmlElement* stateXml = pluginXml->getChildByName("STATE");

        if (pluginMap.contains(pluginName))
        {
            const juce::PluginDescription& pluginDescription = pluginMap[pluginName];
            juce::AudioPluginFormat* format = nullptr;

            for (int i = 0; i < formatManager.getNumFormats(); ++i)
            {
                auto* currentFormat = formatManager.getFormat(i);
                if (currentFormat->getName() == pluginDescription.pluginFormatName)
                {
                    format = currentFormat;
                    break;
                }
            }

            if (format != nullptr)
            {
                format->createPluginInstanceAsync(pluginDescription, 44100.0, 512, [this, stateXml](std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String& error)
                {
                    if (instance != nullptr)
                    {
                        if (stateXml != nullptr)
                        {
                            juce::MemoryBlock stateData;
                            juce::AudioPluginInstance::copyXmlToBinary(*stateXml, stateData);
                            instance->setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));
                        }
                        auto nodeId = audioGraph.addNode(std::move(instance))->nodeID;

                        auto* pluginInstance = audioGraph.getNodeForId(nodeId)->getProcessor();
                        auto editor = pluginInstance->createEditorIfNeeded();
                        if (editor != nullptr)
                        {
                            auto* editorComponent = new PluginEditorComponent(
                                std::unique_ptr<juce::AudioProcessorEditor>(editor),
                                [this, nodeId] { removePluginFromGraph(nodeId); },
                                [this] { resized(); }
                            );
                            editorComponent->setNodeID(nodeId);
                            pluginEditorComponents.add(editorComponent);
                            pluginContainer.addAndMakeVisible(editorComponent);
                            resized();
                        }

                        // Connect the new plugin in the chain
                        if (pluginEditorComponents.size() == 1)
                        {
                            audioGraph.removeConnection({{inputNode->nodeID, 0}, {outputNode->nodeID, 0}});
                            audioGraph.removeConnection({{inputNode->nodeID, 1}, {outputNode->nodeID, 1}});
                            audioGraph.addConnection({{inputNode->nodeID, 0}, {nodeId, 0}});
                            audioGraph.addConnection({{inputNode->nodeID, 1}, {nodeId, 1}});
                        }
                        else
                        {
                            auto previousPluginNodeID = pluginEditorComponents[pluginEditorComponents.size() - 2]->getNodeID();
                            audioGraph.addConnection({{previousPluginNodeID, 0}, {nodeId, 0}});
                            audioGraph.addConnection({{previousPluginNodeID, 1}, {nodeId, 1}});
                        }

                        audioGraph.addConnection({{nodeId, 0}, {outputNode->nodeID, 0}});
                        audioGraph.addConnection({{nodeId, 0}, {outputNode->nodeID, 1}});
                    }
                    else
                    {
                        std::cout << "Failed to create plugin instance: " << error << std::endl;
                    }
                });
            }
        }
    }
}
