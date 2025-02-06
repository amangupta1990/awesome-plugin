#pragma once

#include <JuceHeader.h>

class PluginEditorComponent : public juce::Component
{
public:
    PluginEditorComponent(std::unique_ptr<juce::AudioProcessorEditor> editor, std::function<void()> onDelete)
        : pluginEditor(std::move(editor)), onDelete(onDelete)
    {
        if (pluginEditor != nullptr)
        {
            addAndMakeVisible(pluginEditor.get());
            setSize(pluginEditor->getWidth(), pluginEditor->getHeight());

            deleteButton.setButtonText("Delete");
            deleteButton.onClick = [onDelete] // Capture onDelete by value
            {
                onDelete();
            };
            addAndMakeVisible(deleteButton);
        }
    }

    void resized() override
    {
        if (pluginEditor != nullptr)
        {
            pluginEditor->setBounds(0, 0, getWidth(), getHeight() - 30);
            deleteButton.setBounds(0, getHeight() - 30, getWidth(), 30);
        }
    }

private:
    std::unique_ptr<juce::AudioProcessorEditor> pluginEditor;
    juce::TextButton deleteButton;
    std::function<void()> onDelete;
};