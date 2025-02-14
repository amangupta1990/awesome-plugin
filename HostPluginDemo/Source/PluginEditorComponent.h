#pragma once

#include <JuceHeader.h>

class PluginEditorComponent : public juce::Component
{
public:
    PluginEditorComponent(std::unique_ptr<juce::AudioProcessorEditor> editor, std::function<void()> onClose)
        : editor(std::move(editor)), onClose(std::move(onClose))
    {
        jassert(this->editor != nullptr); // Ensure the editor is valid

        editorHolder.addAndMakeVisible(this->editor.get());
        addAndMakeVisible(editorHolder);
        addAndMakeVisible(deleteButton);

        deleteButton.setButtonText("Delete");

        // ✅ Capture `onClose` properly
        deleteButton.onClick = [this, callback = std::move(onClose)] { callback(); };

        // ✅ Resize editor dynamically with constraints
        updateEditorSize();
    }

    ~PluginEditorComponent() override = default;

    juce::AudioProcessorGraph::NodeID getNodeID() const { return nodeID; }
    void setNodeID(juce::AudioProcessorGraph::NodeID id) { nodeID = id; }

    void resized() override
    {
        updateEditorSize();
    }

    void childBoundsChanged(juce::Component* child) override
    {
        if (child == editor.get())
        {
            updateEditorSize();
        }
    }

private:
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    juce::Component editorHolder; // ✅ Wrapper to hold the editor
    std::function<void()> onClose;
    juce::TextButton deleteButton;
    juce::AudioProcessorGraph::NodeID nodeID;

    void updateEditorSize()
    {
        if (editor == nullptr)
            return;

        int editorWidth = editor->getWidth();
        int editorHeight = editor->getHeight();

        if (editorWidth == 0 || editorHeight == 0)
            return;

        // ✅ Get parent dimensions
        int parentWidth = getParentWidth();
        int parentHeight = getParentHeight();

        // ✅ Keep editors at a **reasonable size**
        int maxWidth = parentWidth * 0.5f;  // Max 50% of available width
        int maxHeight = parentHeight * 0.5f; // Max 50% of available height

        float aspectRatio = static_cast<float>(editorWidth) / editorHeight;

        // ✅ Maintain aspect ratio while constraining the size
        int newWidth = std::min(editorWidth, maxWidth);
        int newHeight = static_cast<int>(newWidth / aspectRatio);

        if (newHeight > maxHeight)
        {
            newHeight = maxHeight;
            newWidth = static_cast<int>(newHeight * aspectRatio);
        }

        // ✅ Keep minimum size so it's never too small
        int minWidth = 300;
        int minHeight = 200;

        newWidth = std::max(newWidth, minWidth);
        newHeight = std::max(newHeight, minHeight);

        // ✅ Set new size, leaving space for delete button
        setSize(newWidth, newHeight + 40); 

        // ✅ Center editor and delete button properly
        editorHolder.setBounds(0, 0, newWidth, newHeight);
        editor->setBounds(0, 0, newWidth, newHeight);

        deleteButton.setBounds((newWidth - 80) / 2, newHeight + 5, 80, 30); // Center delete button
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorComponent)
};
