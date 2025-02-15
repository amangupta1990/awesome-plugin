#pragma once

#include <JuceHeader.h>

class FullScreenView : public juce::Component
{
public:
    FullScreenView(std::unique_ptr<juce::AudioProcessorEditor> editor, std::function<void()> onClose)
        : onClose(std::move(onClose))
    {
        if (editor != nullptr)
        {
            setEditor(std::move(editor));
        }
    }

    void setEditor(std::unique_ptr<juce::AudioProcessorEditor> newEditor)
    {
        editor = std::move(newEditor);
        editorHolder.addAndMakeVisible(editor.get());
        addAndMakeVisible(editorHolder);
        resized();
    }

    std::unique_ptr<juce::AudioProcessorEditor> releaseEditor()
    {
        editorHolder.removeChildComponent(editor.get());
        return std::move(editor);
    }

    void resized() override
    {
        int parentWidth = getWidth();
        int parentHeight = getHeight();

        setBounds(0, 0, parentWidth, parentHeight);
        editorHolder.setBounds(0, 0, parentWidth, parentHeight);
        
        if (editor != nullptr)
        {
            editor->setBounds(0, 0, parentWidth, parentHeight);
        }
    }

private:
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    juce::Component editorHolder; // Wrapper to hold the editor
    std::function<void()> onClose;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FullScreenView)
};
