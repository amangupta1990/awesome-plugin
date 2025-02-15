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

        addAndMakeVisible(titleBar);
        titleBar.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);

        addAndMakeVisible(titleLabel);
        titleLabel.setText("Full Screen View", juce::dontSendNotification);
        titleLabel.setJustificationType(juce::Justification::centred);

        addAndMakeVisible(closeButton);
        closeButton.setButtonText("Close");
        closeButton.onClick = [this] { this->onClose(); };
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

        titleBar.setBounds(0, 0, parentWidth, 50);
        titleLabel.setBounds(0, 50, parentWidth - 80, 30);
        closeButton.setBounds(parentWidth - 80, 50, 80, 30);

        editorHolder.setBounds(0, 80, parentWidth, parentHeight - 80);
        
        if (editor != nullptr)
        {
            editor->setBounds(0, 0, parentWidth, parentHeight - 80);
        }
    }

private:
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    juce::Component editorHolder; // Wrapper to hold the editor
    juce::Label titleBar;
    juce::Label titleLabel;
    juce::TextButton closeButton;
    std::function<void()> onClose;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FullScreenView)
};
