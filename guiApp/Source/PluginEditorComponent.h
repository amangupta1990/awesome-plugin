#pragma once

#include <JuceHeader.h>

class PluginEditorComponent : public juce::Component
{
public:
    PluginEditorComponent(std::unique_ptr<juce::AudioProcessorEditor> editor)
        : pluginEditor(std::move(editor))
    {
        if (pluginEditor != nullptr)
        {
            addAndMakeVisible(pluginEditor.get());
            setSize(pluginEditor->getWidth(), pluginEditor->getHeight());
        }
    }

    void resized() override
    {
        if (pluginEditor != nullptr)
        {
            pluginEditor->setBounds(getLocalBounds());
        }
    }

private:
    std::unique_ptr<juce::AudioProcessorEditor> pluginEditor;
};