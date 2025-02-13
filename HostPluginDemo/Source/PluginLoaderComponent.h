#pragma once

#include <JuceHeader.h>

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
