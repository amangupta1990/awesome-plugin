#pragma once

#include <JuceHeader.h>

//==============================================================================

class PluginEditorComponent final : public Component
{
public:
    template <typename Callback>
    PluginEditorComponent (std::unique_ptr<AudioProcessorEditor> editorIn, Callback&& onClose)
        : editor (std::move (editorIn))
    {
        addAndMakeVisible (editor.get());
        addAndMakeVisible (closeButton);

        childBoundsChanged (editor.get());

        closeButton.onClick = std::forward<Callback> (onClose);
    }

    void setScaleFactor (float scale)
    {
        if (editor != nullptr)
            editor->setScaleFactor (scale);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        editor->setBounds(area.removeFromTop(area.getHeight() - buttonHeight));
        closeButton.setBounds(area);
    }

    void childBoundsChanged (Component* child) override
    {
        if (child != editor.get())
            return;

        const auto size = editor != nullptr ? editor->getLocalBounds()
                                            : Rectangle<int>();

        setSize (size.getWidth(), margin + buttonHeight + size.getHeight());
    }

private:
    static constexpr auto buttonHeight = 40;
    static constexpr auto margin = 10;

    std::unique_ptr<AudioProcessorEditor> editor;
    TextButton closeButton { "Close Plugin" };
};

//==============================================================================

class ScaledDocumentWindow final : public DocumentWindow
{
public:
    ScaledDocumentWindow (Colour bg, float scale)
        : DocumentWindow ("Editor", bg, 0), desktopScale (scale) {}

    float getDesktopScaleFactor() const override { return Desktop::getInstance().getGlobalScaleFactor() * desktopScale; }

private:
    float desktopScale = 1.0f;
};

//==============================================================================

class PluginLoaderComponent final : public Component
{
public:
    template <typename Callback>
    PluginLoaderComponent (AudioPluginFormatManager& manager,
                           KnownPluginList& list,
                           Callback&& callback)
        : pluginListComponent (manager, list, {}, {})
    {
        pluginListComponent.getTableListBox().setMultipleSelectionEnabled (false);

        addAndMakeVisible (pluginListComponent);
        addAndMakeVisible (addButton);

        addButton.onClick = [this, &list, cb = std::forward<Callback> (callback)]
        {
            const auto index = pluginListComponent.getTableListBox().getSelectedRow();
            const auto& types = list.getTypes();

            if (isPositiveAndBelow (index, types.size()))
                NullCheckedInvocation::invoke (cb, types.getReference (index));
        };
    }

    void resized() override
    {
        auto area = getLocalBounds();
        pluginListComponent.setBounds(area.removeFromTop(area.getHeight() - 40));
        addButton.setBounds(area);
    }

private:
    PluginListComponent pluginListComponent;
    TextButton addButton { "Add to Chain" };
};
