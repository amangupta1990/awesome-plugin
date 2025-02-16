#pragma once

#include <JuceHeader.h>
#include "FullScreenView.h"

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
    return R"(
<svg width="12" height="12" viewBox="0 0 12 12" fill="none" xmlns="http://www.w3.org/2000/svg">
<path d="M11 1L6 6M6 6L1 11M6 6L1 1M6 6L11 11" stroke="#8D8D8D" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
</svg>

    )";
}

juce::String PluginEditorComponent::expandButtonSvg() const
{
    return R"(
       <svg width="9" height="12" viewBox="0 0 9 12" fill="none" xmlns="http://www.w3.org/2000/svg">
<g opacity="0.4">
<path d="M1 4.5L4.5 1L8 4.5" stroke="white"/>
<path d="M1 7.5L4.5 11L8 7.5" stroke="white"/>
</g>
</svg>

    )";
}

juce::String PluginEditorComponent::closeButtonSvg() const
{
    return R"(
        <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
            <line x1="12" y1="12" x2="28" y2="28" stroke="white" stroke-width="2"/>
            <line x1="12" y1="28" x2="28" y2="12" stroke="white" stroke-width="2"/>
        </svg>
    )";
}
