#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    void populateVSTComboBox();
    void vstSelected();

    juce::ComboBox vstComboBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};