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

        addAndMakeVisible(closeButton);
        auto closeSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(closeButtonSvg()));
        closeButton.setImages(closeSvg.get());
        closeButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);
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

        titleBar.setBounds(0, 0, parentWidth, 80);
        closeButton.setBounds(parentWidth - 50, 30, 40, 40);

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
    juce::DrawableButton closeButton { "Close", juce::DrawableButton::ButtonStyle::ImageFitted };
    std::function<void()> onClose;

    juce::String closeButtonSvg() const
    {
        return R"(
            <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                <circle cx="20" cy="20" r="18" stroke="white" stroke-width="2" fill="none"/>
            </svg>
        )";
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FullScreenView)
};
