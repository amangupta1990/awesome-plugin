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
        setUsingNativeTitleBar(true);
        setResizable(false, false);
        setSize(600, 400);

        // Create and set the content component
        auto* content = new SelectPluginDialog(pluginMap, callback);
        setContentOwned(content, true);

        centreWithSize(getWidth(), getHeight());
        setVisible(true);
        addToDesktop();  // Ensure it is attached to the desktop
    }

    ~PluginDialogWindow() override {}

    void closeButtonPressed() override
    {
        setVisible(false);  // Hide instead of deleting immediately
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginDialogWindow)
};
