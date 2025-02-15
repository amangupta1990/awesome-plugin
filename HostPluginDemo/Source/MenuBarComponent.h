#pragma once

#include <JuceHeader.h>

class MenuBarComponent : public juce::Component,
                         public juce::MenuBarModel
{
public:
    MenuBarComponent(juce::ApplicationCommandManager* commandManager, juce::AudioDeviceManager& dm)
        : commandManager(commandManager), deviceManager(dm) // ✅ Initialize the reference properly
    {
        setApplicationCommandManagerToWatch(commandManager);

        addAndMakeVisible(muteButton);
        muteButton.setButtonText("Mute");
        muteButton.onClick = [this] { toggleMute(); };

        addAndMakeVisible(bypassButton);
        bypassButton.setButtonText("Bypass");
        bypassButton.onClick = [this] { toggleBypass(); };

        addAndMakeVisible(menuBar);
        menuBar.setModel(this);
    }
    
    bool isMuted(){
        return muted;
    }
    
    bool isBypassed(){
        return bypassed;
    }
    
    void setInputSource(juce::String src){
        inputSource = src;
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

    ~MenuBarComponent() override {}

    void resized() override
    {
        auto area = getLocalBounds();
        auto buttonWidth = 80;
        auto buttonHeight = 20;
        muteButton.setBounds(area.removeFromRight(buttonWidth).removeFromBottom(buttonHeight).reduced(5));
        bypassButton.setBounds(area.removeFromRight(buttonWidth).removeFromBottom(buttonHeight).reduced(5));
        menuBar.setBounds(area);
    }

    // Menu bar related methods
    juce::StringArray getMenuBarNames()
    {
        return {"File", "Edit", "Options"};
    }

    juce::PopupMenu getMenuForIndex(int /*menuIndex*/, const juce::String &menuName)
    {
        juce::PopupMenu menu;

        if (menuName == "Options")
        {
            juce::StringArray inputDevices = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(true);
            juce::StringArray outputDevices = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(false);

            menu.addSectionHeader("Input Devices");
            auto currentInputDevice = deviceManager.getCurrentAudioDevice()->getName();
            for (int i = 0; i < inputDevices.size(); ++i)
            {
                menu.addItem(1000 + i, inputDevices[i], true, currentInputDevice == inputDevices[i]);
            }

            menu.addSectionHeader("Output Devices");
            auto currentOutputDevice = deviceManager.getCurrentAudioDevice()->getName();
            for (int i = 0; i < outputDevices.size(); ++i)
            {
                menu.addItem(2000 + i, outputDevices[i], true, currentOutputDevice == outputDevices[i]);
            }
        }

        return menu;
    }

    void menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/)
    {
        if (menuItemID >= 1000 && menuItemID < 2000)
        {
            int inputIndex = menuItemID - 1000;
            auto inputDeviceName = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(true)[inputIndex];
            auto setup = deviceManager.getAudioDeviceSetup();
            setup.inputDeviceName = inputDeviceName;
            deviceManager.setAudioDeviceSetup(setup, true);
        }
        else if (menuItemID >= 2000)
        {
            int outputIndex = menuItemID - 2000;
            auto outputDeviceName = deviceManager.getAvailableDeviceTypes()[0]->getDeviceNames(false)[outputIndex];
            auto setup = deviceManager.getAudioDeviceSetup();
            setup.outputDeviceName = outputDeviceName;
            deviceManager.setAudioDeviceSetup(setup, true);
        }
    }

    juce::MenuBarComponent menuBar;

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
    juce::String inputSource = " ";

    juce::TextButton muteButton;
    juce::TextButton bypassButton;
    

    std::function<void(bool)> muteCallback = nullptr;
    std::function<void(bool)> bypassCallback = nullptr;

    juce::AudioDeviceManager& deviceManager;
    juce::ApplicationCommandManager* commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarComponent)
};
