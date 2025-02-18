#pragma once

#include <JuceHeader.h>
#include "FullScreenView.h"
#include "Buttons.h"

class PluginEditorComponent : public juce::Component
{
public:
    PluginEditorComponent(std::unique_ptr<juce::AudioProcessorEditor> editor, std::function<void()> onClose, std::function<void()> parentResized)
        : editor(std::move(editor)), onClose(std::move(onClose)), parentResized(std::move(parentResized)), isFullscreen(false), isBypassed(false)
    {
        jassert(this->editor != nullptr); // Ensure the editor is valid

        editorHolder.addAndMakeVisible(this->editor.get());
        addAndMakeVisible(editorHolder);
        addAndMakeVisible(deleteButton);
        addAndMakeVisible(expandButton);
        addAndMakeVisible(bypassButton);
        addChildComponent(closeButton); // Initially hidden

        // Ensure SVG functions are declared before usage
        auto deleteSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(deleteButtonSvg()));
        auto expandSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(expandButtonSvg()));
        auto closeSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(closeButtonSvg()));
        auto bypassSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(bypassButtonOffSvg())); // Start with the off state

        deleteButton.setImages(deleteSvg.get());
        expandButton.setImages(expandSvg.get());
        closeButton.setImages(closeSvg.get());
        bypassButton.setImages(bypassSvg.get());

        deleteButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);
        expandButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);
        closeButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);
        bypassButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);

        deleteButton.setClickingTogglesState(false);
        expandButton.setClickingTogglesState(false);
        closeButton.setClickingTogglesState(false);
        bypassButton.setClickingTogglesState(false);

        deleteButton.onClick = [this, callback = std::move(onClose)] { callback(); };
        expandButton.onClick = [this] { toggleFullscreen(); };
        closeButton.onClick = [this] { exitFullscreen(); };
        bypassButton.onClick = [this] { toggleBypass(); };

        fullScreenView = std::make_unique<FullScreenView>(nullptr, [this] { exitFullscreen(); });
        fullScreenView->setVisible(false);

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

    juce::AudioProcessor* getAudioProcessor()
    {
        return editor->getAudioProcessor();
    }

private:
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    juce::Component editorHolder; // Wrapper to hold the editor
    std::function<void()> onClose;
    std::function<void()> parentResized;
    juce::DrawableButton deleteButton { "Delete", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::DrawableButton expandButton { "Expand", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::DrawableButton closeButton { "Close", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::DrawableButton bypassButton { "Bypass", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::AudioProcessorGraph::NodeID nodeID;

    bool isFullscreen;
    bool isBypassed;
    juce::Rectangle<int> previousBounds;
    std::unique_ptr<FullScreenView> fullScreenView;

    void updateEditorSize()
    {
        if (editor == nullptr)
            return;

        int parentWidth = getParentWidth();
        int parentHeight = getParentHeight();

        if (isFullscreen)
        {
            setBounds(0, 0, parentWidth, parentHeight);
            editorHolder.setBounds(20, 0, parentWidth - 40, parentHeight - 40);
            editor->setBounds(0, 0, parentWidth - 40, parentHeight - 40);
            closeButton.setBounds(parentWidth - 50, 10, 40, 40);
            closeButton.setVisible(true);
            expandButton.setVisible(false);
            deleteButton.setVisible(false);
            bypassButton.setVisible(false);
        }
        else
        {
            int editorWidth = editor->getWidth();
            int editorHeight = editor->getHeight();

            int newWidth = juce::jmin(editorWidth / 1.5, 400 / 1.5);
            int newHeight = juce::jmin(editorHeight / 1.5, 300 / 1.5);

            int centerX = (parentWidth - newWidth) / 2;
            int centerY = (parentHeight - newHeight - 70) / 2;

            setBounds(centerX, centerY, newWidth, newHeight + 70);
            editorHolder.setBounds(0, 50, newWidth, newHeight);
            editor->setBounds(0, 0, newWidth, newHeight);

            int buttonSize = 30;
            int buttonSpacing = 20;

            deleteButton.setBounds(0, 0, buttonSize, buttonSize);
            expandButton.setBounds(buttonSize + buttonSpacing, 0, buttonSize, buttonSize);
            bypassButton.setBounds(newWidth - buttonSize, 0, buttonSize, buttonSize);

            closeButton.setVisible(false);
            expandButton.setVisible(true);
            deleteButton.setVisible(true);
            bypassButton.setVisible(true);
        }
    }

    void toggleFullscreen()
    {
        if (!isFullscreen)
        {
            previousBounds = getBounds();
            isFullscreen = true;

            fullScreenView->setEditor(std::move(editor)); // Move editor to fullscreen
            editor.reset(); // Clear local reference
            fullScreenView->setVisible(true);

            if (auto* topLevelComp = getTopLevelComponent())
            {
                topLevelComp->addAndMakeVisible(fullScreenView.get());
                fullScreenView->setBounds(topLevelComp->getLocalBounds());
                fullScreenView->toFront(true);
            }

            // Ensure close button is always in the front
            addAndMakeVisible(closeButton);
            closeButton.toFront(false);
            closeButton.setVisible(true);

            DBG("PluginEditorComponent: Fullscreen mode activated");
        }
        else
        {
            exitFullscreen();
        }
    }

    void exitFullscreen()
    {
        if (isFullscreen)
        {
            isFullscreen = false;

            // Restore editor from FullScreenView
            editor = fullScreenView->releaseEditor();

            if (editor)
            {
                editorHolder.addAndMakeVisible(editor.get());
            }

            fullScreenView->setVisible(false);

            if (auto* topLevelComp = getTopLevelComponent())
            {
                topLevelComp->removeChildComponent(fullScreenView.get());
            }

            // Restore plugin layout
            setBounds(previousBounds);
            addAndMakeVisible(editorHolder);
            addAndMakeVisible(deleteButton);
            addAndMakeVisible(expandButton);
            addAndMakeVisible(bypassButton);
            closeButton.setVisible(false);

            updateEditorSize();

            parentResized();
        }
    }

    void toggleBypass()
    {
        if (auto* processor = getAudioProcessor())
        {
            isBypassed = !isBypassed;
            if (auto* bypassParam = processor->getBypassParameter())
            {
                bypassParam->setValueNotifyingHost(isBypassed ? 1.0f : 0.0f);
            }

            auto bypassSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(isBypassed ? bypassButtonOnSvg() : bypassButtonOffSvg()));
            bypassButton.setImages(bypassSvg.get());
        }
    }

    // Ensure SVG functions are declared before use
    juce::String deleteButtonSvg() const;
    juce::String expandButtonSvg() const;
    juce::String closeButtonSvg() const;
    juce::String bypassButtonSvg() const;
    juce::String bypassButtonOnSvg() const;
    juce::String bypassButtonOffSvg() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorComponent)
};

juce::String PluginEditorComponent::deleteButtonSvg() const
{
    return SVGButtons::deleteButtonSVG;
}

juce::String PluginEditorComponent::expandButtonSvg() const
{
    return SVGButtons::expandButtonSVG;
}

juce::String PluginEditorComponent::closeButtonSvg() const
{
    return SVGButtons::closeButtonSVG;
}

juce::String PluginEditorComponent::bypassButtonOnSvg() const
{
    return SVGButtons::bypassPluginOnSVG;
}

juce::String PluginEditorComponent::bypassButtonOffSvg() const
{
    return SVGButtons::bypassPluginOffSVG;
}
