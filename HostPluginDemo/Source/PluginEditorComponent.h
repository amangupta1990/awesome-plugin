#pragma once

#include <JuceHeader.h>
#include "FullScreenView.h"

class PluginEditorComponent : public juce::Component
{
public:
    PluginEditorComponent(std::unique_ptr<juce::AudioProcessorEditor> editor, std::function<void()> onClose)
        : editor(std::move(editor)), onClose(std::move(onClose)), isFullscreen(false)
    {
        jassert(this->editor != nullptr); // Ensure the editor is valid

        editorHolder.addAndMakeVisible(this->editor.get());
        addAndMakeVisible(editorHolder);
        addAndMakeVisible(deleteButton);
        addAndMakeVisible(expandButton);
        addChildComponent(closeButton); // Initially hidden

        // ✅ FIX: Now deleteButtonSvg() and expandButtonSvg() are recognized
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
        addAndMakeVisible(fullScreenView.get());

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
            editorHolder.setBounds(20, 20, parentWidth - 40, parentHeight - 80);
            editor->setBounds(0, 0, parentWidth - 40, parentHeight - 80);
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
            editorHolder.setBounds(0, 70, newWidth, newHeight);
            editor->setBounds(0, 0, newWidth, newHeight);

            int buttonSize = 40;
            int buttonX = (newWidth - 90) / 2;

            deleteButton.setBounds(buttonX, 10, buttonSize, buttonSize);
            expandButton.setBounds(buttonX + buttonSize + 15, 10, buttonSize, buttonSize);
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
            fullScreenView->setEditor(std::move(editor));
            fullScreenView->setVisible(true);
            fullScreenView->toFront(true);
            fullScreenView->setBounds(0, 0, getParentWidth(), getParentHeight());
        }
        else
        {
            isFullscreen = false;
            editor = fullScreenView->releaseEditor();
            fullScreenView->setVisible(false);
        }
        updateEditorSize();
    }

    void exitFullscreen()
    {
        isFullscreen = false;
        editor = fullScreenView->releaseEditor();
        fullScreenView->setVisible(false);
        setBounds(previousBounds);
        updateEditorSize();
    }

    // ✅ FIX: Declare deleteButtonSvg and expandButtonSvg inside the class
    juce::String deleteButtonSvg() const
    {
        return R"(
            <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                <circle cx="20" cy="20" r="18" stroke="white" stroke-width="2" fill="none"/>
                <line x1="12" y1="12" x2="28" y2="28" stroke="white" stroke-width="2"/>
                <line x1="12" y1="28" x2="28" y2="12" stroke="white" stroke-width="2"/>
            </svg>
        )";
    }

    juce::String expandButtonSvg() const
    {
        return R"(
            <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                <circle cx="20" cy="20" r="18" stroke="white" stroke-width="2" fill="none"/>
                <line x1="16" y1="16" x2="8" y2="8" stroke="white" stroke-width="2"/>
                <line x1="24" y1="16" x2="32" y2="8" stroke="white" stroke-width="2"/>
                <line x1="16" y1="24" x2="8" y2="32" stroke="white" stroke-width="2"/>
                <line x1="24" y1="24" x2="32" y2="32" stroke="white" stroke-width="2"/>
            </svg>
        )";
    }

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorComponent)
};