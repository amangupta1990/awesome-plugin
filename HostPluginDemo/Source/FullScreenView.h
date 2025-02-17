#pragma once

#include <JuceHeader.h>
#include "Buttons.h"

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
        auto closeSvgXml = juce::parseXML(getCloseButtonSVG());
        if (closeSvgXml != nullptr)
        {
            auto closeSvg = juce::Drawable::createFromSVG(*closeSvgXml);
            closeButton.setImages(closeSvg.get());
        }
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

    juce::String getCloseButtonSVG(); // Declaration of the method

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FullScreenView)
};


juce::String FullScreenView::getCloseButtonSVG()
{
    return SVGButtons::closeButtonSVG;
}
