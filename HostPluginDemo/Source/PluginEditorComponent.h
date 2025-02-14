#pragma once

#include <JuceHeader.h>

class PluginEditorComponent : public juce::Component
{
public:
    PluginEditorComponent(std::unique_ptr<juce::AudioProcessorEditor> editor, std::function<void()> onClose)
        : editor(std::move(editor)), onClose(onClose)
    {
        addAndMakeVisible(this->editor.get());
        addAndMakeVisible(deleteButton);
        deleteButton.setButtonText("Delete");
        deleteButton.onClick = [this, onClose]
        { onClose(); };                                                    // Explicitly capture onClose
        setSize(this->editor->getWidth(), this->editor->getHeight() + 30); // Add space for the delete button
    }

    juce::AudioProcessorGraph::NodeID getNodeID() const { return nodeID; }
    void setNodeID(juce::AudioProcessorGraph::NodeID id) { nodeID = id; }

    void setScaleFactor(float scale)
    {
        if (editor != nullptr)
            editor->setScaleFactor(scale);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        editor->setBounds(area.removeFromTop(area.getHeight() - 30));             // Adjust for delete button
        deleteButton.setBounds(area); // Slight gap
    }

    void childBoundsChanged(juce::Component* child) override
    {
        if (child != editor.get())
            return;

        const auto size = editor != nullptr ? editor->getLocalBounds()
                                            : juce::Rectangle<int>();

        setSize(size.getWidth(), 30 + size.getHeight());
    }

private:
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    std::function<void()> onClose;
    juce::TextButton deleteButton;
    juce::AudioProcessorGraph::NodeID nodeID;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorComponent)
};