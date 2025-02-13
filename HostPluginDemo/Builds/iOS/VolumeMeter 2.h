#pragma once

#include <JuceHeader.h>

class VolumeMeter : public juce::Component, private juce::Timer
{
public:
    VolumeMeter()
    {
        addAndMakeVisible(muteButton);
        muteButton.setButtonText("Mute");
        muteButton.onClick = [this]
        { toggleMute(); };

        addAndMakeVisible(bypassButton);
        bypassButton.setButtonText("Bypass");
        bypassButton.onClick = [this]
        { toggleBypass(); };

        startTimerHz(30); // Update the meter 30 times per second
    }

    void setMuteCallback(std::function<void(bool)> callback)
    {
        muteCallback = std::move(callback);
    }

    void setBypassCallback(std::function<void(bool)> callback)
    {
        bypassCallback = std::move(callback);
    }


    void setBypassed(bool bypass)
    {
        bypassed = bypass;
        bypassButton.setColour(juce::TextButton::buttonColourId, bypassed ? juce::Colours::red : juce::Colours::green);
    }

    void setMuted(bool mute)
    {
        muted = mute;
        muteButton.setColour(juce::TextButton::buttonColourId, muted ? juce::Colours::red : juce::Colours::green);
    }

    ~VolumeMeter() override {}

    void setInputSource(const juce::String &source)
    {
        inputSource = source;
        repaint();
    }

    void setLevel(float newLevel)
    {
        if (level != newLevel)
        {
            level = newLevel;
            clipping = (level > 1.0f); // Assuming 1.0f is the clipping threshold
            repaint();
        }
    }

    bool isMuted() const { return muted; }
    bool isBypassed() const { return bypassed; }

    void paint(juce::Graphics &g) override
    {
        g.fillAll(juce::Colours::black);

        auto meterWidth = getWidth() * level;
        g.setColour(clipping ? juce::Colours::red : juce::Colours::green);
        g.fillRect(0, 0, static_cast<int>(meterWidth), getHeight() - 40); // Adjust height for buttons

        g.setColour(juce::Colours::white);
        g.drawText(inputSource, getLocalBounds().removeFromTop(20), juce::Justification::centredTop, true);

        // Draw dB level markings
        g.setColour(juce::Colours::white);
        for (int i = 0; i <= 10; ++i)
        {
            float db = juce::Decibels::gainToDecibels(i / 10.0f);
            int x = static_cast<int>(getWidth() * (i / 10.0f));
            g.drawLine(x, getHeight() - 50, x, getHeight() - 40); // Adjust height for buttons
            g.drawText(juce::String(db, 1) + " dB", x - 15, getHeight() - 60, 30, 10, juce::Justification::centred);
        }
    }

    void resized() override
    {
        auto area = getLocalBounds();
        auto buttonWidth = 80;
        auto buttonHeight = 20;
        muteButton.setBounds(area.removeFromRight(buttonWidth).removeFromBottom(buttonHeight).reduced(5));
        bypassButton.setBounds(area.removeFromRight(buttonWidth).removeFromBottom(buttonHeight).reduced(5));
    }

    void timerCallback() override
    {
        // Timer callback implementation
    }

private:
    void toggleMute()
    {
        muted = !muted;
        muteButton.setColour(juce::TextButton::buttonColourId, muted ? juce::Colours::red : juce::Colours::green);
        if (muteCallback)
            muteCallback(muted);
    }

    void toggleBypass()
    {
        bypassed = !bypassed;
        bypassButton.setColour(juce::TextButton::buttonColourId, bypassed ? juce::Colours::red : juce::Colours::green);
        if (bypassCallback)
            bypassCallback(bypassed);
    }

    bool muted = false;
    bool bypassed = false;
    float level = 0.0f;
    bool clipping = false;
    juce::String inputSource = "Input";

    juce::TextButton muteButton;
    juce::TextButton bypassButton;

    std::function<void(bool)> muteCallback = nullptr;
    std::function<void(bool)> bypassCallback = nullptr;
};