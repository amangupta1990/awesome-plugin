#pragma once

#include <JuceHeader.h>
#include "SelectPluginDialog.h"

class PluginDialogWindow : public juce::DocumentWindow
{
public:
PluginDialogWindow(const juce::String& name,
    const juce::HashMap<juce::String, juce::PluginDescription>& pluginMap,
    SelectPluginDialog::PluginSelectedCallback callback)
: juce::DocumentWindow(name, juce::Colours::lightgrey, juce::DocumentWindow::closeButton)
{
std::cout << "PluginDialogWindow constructor called" << std::endl;

setUsingNativeTitleBar(true);
setResizable(false, false);

// Create the plugin selection component
auto* content = new SelectPluginDialog(pluginMap, callback);
setContentOwned(content, true);

// Ensure the window is centered and correctly sized
centreWithSize(600, 400);

setVisible(true);
toFront(true);

std::cout << "PluginDialogWindow should now be visible with correct size" << std::endl;
setTitle(name);
}


    ~PluginDialogWindow() override {}

    void closeButtonPressed() override
    {
        setVisible(false);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginDialogWindow)
};
