#pragma once

#include <JuceHeader.h>
#include <functional>

class SelectPluginDialog : public juce::Component,
                           public juce::Button::Listener,
                           public juce::TableListBoxModel
{
public:
    using PluginSelectedCallback = std::function<void(const juce::String&)>;

    SelectPluginDialog(const juce::HashMap<juce::String, juce::PluginDescription>& pluginMap, PluginSelectedCallback callback)
        : onPluginSelected(callback)
    {
        // Title label
        std::cout << "SelectPluginDialog constructor called" << std::endl;
        titleLabel.setText("Select a VST Plugin", juce::dontSendNotification);
        titleLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(titleLabel);

        // Plugin Table
        addAndMakeVisible(pluginTable);
        pluginTable.setModel(this);
        pluginTable.getHeader().addColumn("Type", 1, 150);
        pluginTable.getHeader().addColumn("Manufacturer", 2, 150);
        pluginTable.getHeader().addColumn("Plugin", 3, 300);

        // Add button
        addButton.setButtonText("Add Plugin");
        addButton.addListener(this);
        addAndMakeVisible(addButton);

        // Close button
        closeButton.setButtonText("Close");
        closeButton.addListener(this);
        addAndMakeVisible(closeButton);

        // Overlay
        overlay.setOpaque(true);
        overlay.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));
        addAndMakeVisible(overlay);

        // Populate the table with the plugin map
        setPluginList(pluginMap);
      
    }

    ~SelectPluginDialog() override
    {
        addButton.removeListener(this);
        closeButton.removeListener(this);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::white);
    }

    void resized() override
    {
        auto area = getTopLevelComponent()->getBounds();
        titleLabel.setBounds(area.removeFromTop(40));
        pluginTable.setBounds(area.removeFromTop(getHeight() - 100).reduced(10));
        addButton.setBounds(area.removeFromBottom(40).reduced(10));
        closeButton.setBounds(area.removeFromBottom(40).reduced(10));
        overlay.setBounds(getTopLevelComponent()->getBounds());
    }

    void buttonClicked(juce::Button* button) override
    {
        if (button == &addButton)
        {
            auto selectedPlugin = getSelectedPlugin();
            if (onPluginSelected && !selectedPlugin.isEmpty())
            {
                onPluginSelected(selectedPlugin);
            }
        }
        else if (button == &closeButton)
        {
            setVisible(false);
        }
    }

    int getNumRows() override
    {
        return plugins.size();
    }

    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(juce::Colours::lightblue);
        else
            g.fillAll(juce::Colours::white);
    }

    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override
    {
        if (rowNumber < plugins.size())
        {
            const auto& plugin = plugins[rowNumber];
            if (columnId == 1)
                g.drawText(plugin.type, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
            else if (columnId == 2)
                g.drawText(plugin.manufacturer, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
            else if (columnId == 3)
                g.drawText(plugin.name, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }
    }

    void setPluginList(const juce::HashMap<juce::String, juce::PluginDescription>& pluginMap)
    {
        plugins.clear();
        for (auto it = pluginMap.begin(); it != pluginMap.end(); ++it)
        {
            const auto& pluginDescription = it.getValue();
            PluginInfo plugin;
            plugin.type = pluginDescription.pluginFormatName;
            plugin.manufacturer = pluginDescription.manufacturerName;
            plugin.name = pluginDescription.name;
            plugins.add(plugin);
        }
        pluginTable.updateContent();
    }

    juce::String getSelectedPlugin() const
    {
        auto selectedRow = pluginTable.getSelectedRow();
        if (selectedRow >= 0 && selectedRow < plugins.size())
            return plugins[selectedRow].name;
        return {};
    }

private:
    struct PluginInfo
    {
        juce::String type;
        juce::String manufacturer;
        juce::String name;
    };

    juce::Label titleLabel;
    juce::TableListBox pluginTable;
    juce::TextButton addButton;
    juce::TextButton closeButton;
    juce::Component overlay;

    juce::Array<PluginInfo> plugins;
    PluginSelectedCallback onPluginSelected;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectPluginDialog)
};
