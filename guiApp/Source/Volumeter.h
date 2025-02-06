#pragma once

#include <JuceHeader.h>

class VolumeMeter : public juce::Component, public juce::Timer
{
public:
    VolumeMeter()
    {
        startTimerHz(30); // Update the meter 30 times per second
    }

    void setLevel(float newLevel)
    {
        level = newLevel;
        repaint();
    }

    void paint(juce::Graphics &g) override
    {
        g.fillAll(juce::Colours::black);

        auto meterHeight = getHeight() * level;
        g.setColour(juce::Colours::green);
        g.fillRect(0, static_cast<int>(getHeight()) - static_cast<int>(meterHeight), getWidth(), static_cast<int>(meterHeight));
    }

    void timerCallback() override
    {
        // Simulate input level for demonstration purposes
        setLevel(juce::jlimit(0.0f, 1.0f, level * 0.95f + 0.05f * juce::Random::getSystemRandom().nextFloat()));
    }

private:
    float level = 0.0f;
};