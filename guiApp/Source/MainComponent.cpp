#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(800, 600);

    // Add the ComboBox to the component
    addAndMakeVisible(vstComboBox);
    vstComboBox.onChange = [this]
    { vstSelected(); };

    // Populate the ComboBox with VST plugins
    populateVSTComboBox();
}

MainComponent::~MainComponent() {}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::black);
    g.setFont(20.0f);
    g.drawText("Hello, JUCE!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // Set the bounds for the ComboBox
    vstComboBox.setBounds(10, 10, getWidth() - 20, 30);
}

void MainComponent::populateVSTComboBox()
{
    // Scan for VST plugins and add them to the ComboBox
    // This is a placeholder implementation
    vstComboBox.addItem("VST Plugin 1", 1);
    vstComboBox.addItem("VST Plugin 2", 2);
    vstComboBox.addItem("VST Plugin 3", 3);
}

void MainComponent::vstSelected()
{
    // Handle VST selection
    auto selectedVST = vstComboBox.getText();
    DBG("Selected VST: " + selectedVST);
}