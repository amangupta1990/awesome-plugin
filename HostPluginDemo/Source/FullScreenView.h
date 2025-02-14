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

        addAndMakeVisible(closeButton);

        auto closeSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(closeButtonSvg()));
        closeButton.setImages(closeSvg.get());
        closeButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);
        closeButton.setClickingTogglesState(false);
        closeButton.onClick = [this] { this->onClose(); };

        setVisible(false);
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
        int parentWidth = getParentWidth();
        int parentHeight = getParentHeight();

        setBounds(0, 0, parentWidth, parentHeight);
        editorHolder.setBounds(0, 0, parentWidth, parentHeight);
        if (editor != nullptr)
        {
            editor->setBounds(0, 0, parentWidth, parentHeight);
        }
        closeButton.setBounds(parentWidth - 50, 10, 40, 40);
    }

private:
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    juce::Component editorHolder; // Wrapper to hold the editor
    juce::DrawableButton closeButton { "Close", juce::DrawableButton::ButtonStyle::ImageFitted };
    std::function<void()> onClose;

    juce::String closeButtonSvg() const
    {
        return R"(
            <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                <circle cx="20" cy="20" r="18" stroke="white" stroke-width="2" fill="none"/>
                <line x1="12" y1="12" x2="28" y2="28" stroke="white" stroke-width="2"/>
                <line x1="12" y1="28" x2="28" y2="12" stroke="white" stroke-width="2"/>
            </svg>
        )";
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FullScreenView)
};
