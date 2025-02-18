#pragma once

#include <JuceHeader.h>
#include "FullScreenView.h"
#include "Buttons.h"

class PluginEditorComponent : public juce::Component
{
public:
    PluginEditorComponent(std::unique_ptr<juce::AudioProcessorEditor> editor, std::function<void()> onClose, std::function<void()> parentResized)
        : editor(std::move(editor)), onClose(std::move(onClose)), parentResized(std::move(parentResized)), isFullscreen(false)
    {
        jassert(this->editor != nullptr); // Ensure the editor is valid

        editorHolder.addAndMakeVisible(this->editor.get());
        addAndMakeVisible(editorHolder);
        addAndMakeVisible(deleteButton);
        addAndMakeVisible(expandButton);
        addChildComponent(closeButton); // Initially hidden

        // Ensure SVG functions are declared before usage
        auto deleteSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(deleteButtonSvg()));
        auto expandSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(expandButtonSvg()));
        auto closeSvg = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(closeButtonSvg()));

        deleteButton.setImages(deleteSvg.get());
        expandButton.setImages(expandSvg.get());
        closeButton.setImages(closeSvg.get());

        deleteButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);
        expandButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);
        closeButton.setButtonStyle(juce::DrawableButton::ButtonStyle::ImageFitted);

        deleteButton.setClickingTogglesState(false);
        expandButton.setClickingTogglesState(false);
        closeButton.setClickingTogglesState(false);

        deleteButton.onClick = [this, callback = std::move(onClose)] { callback(); };
        expandButton.onClick = [this] { toggleFullscreen(); };
        closeButton.onClick = [this] { exitFullscreen(); };

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
    juce::AudioProcessorGraph::NodeID nodeID;

    bool isFullscreen;
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
        }
        else
        {
            int editorWidth = editor->getWidth();
            int editorHeight = editor->getHeight();

            int newWidth = juce::jmin(editorWidth, 400);
            int newHeight = juce::jmin(editorHeight, 300);

            int centerX = (parentWidth - newWidth) / 2;
            int centerY = (parentHeight - newHeight - 70) / 2;

            setBounds(centerX, centerY, newWidth, newHeight + 70);
            editorHolder.setBounds(0, 50, newWidth, newHeight);
            editor->setBounds(0, 0, newWidth, newHeight);

            int buttonSize = 30;
            int buttonX = (newWidth - 90) / 2;

            deleteButton.setBounds(buttonX, 0, buttonSize, buttonSize);
            expandButton.setBounds(buttonX + buttonSize + 20, 0, buttonSize, buttonSize);
            closeButton.setVisible(false);
            expandButton.setVisible(true);
            deleteButton.setVisible(true);
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
            closeButton.setVisible(false);

            updateEditorSize();

            parentResized();
        }
    }

    // Ensure SVG functions are declared before use
    juce::String deleteButtonSvg() const;
    juce::String expandButtonSvg() const;
    juce::String closeButtonSvg() const;

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
