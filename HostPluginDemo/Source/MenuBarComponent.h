#pragma once

#include <JuceHeader.h>
#include "Buttons.h"

class MenuBarComponent : public juce::Component,
                         public juce::MenuBarModel
{
public:
    MenuBarComponent(juce::ApplicationCommandManager* commandManager, juce::AudioDeviceManager& dm)
        : commandManager(commandManager), deviceManager(dm) // Initialize the reference properly
    {
        setApplicationCommandManagerToWatch(commandManager);

        addAndMakeVisible(muteButton);
        muteButton.onClick = [this] { toggleMute(); };
        updateMuteButton();

        addAndMakeVisible(bypassButton);
        bypassButton.onClick = [this] { toggleBypass(); };
        updateBypassButton();

        addAndMakeVisible(menuBar);
        menuBar.setModel(this);

        // Ensure buttons are in front
        muteButton.toFront(true);
        bypassButton.toFront(true);
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
        updateBypassButton();
    }

    void setMuted(bool mute)
    {
        muted = mute;
        updateMuteButton();
    }

    ~MenuBarComponent() override {}

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour::fromRGB(38, 38, 38)); // Set dark grey background color using RGB
    }

    void resized() override
    {
        auto area = getLocalBounds();
        auto buttonWidth = 160; // 2X bigger
        auto buttonHeight = 80; // 2X bigger
        auto menuBarHeight = 20;

        menuBar.setBounds(area.removeFromTop(menuBarHeight));
        menuBar.setColour(juce::Label::backgroundColourId, juce::Colour::fromRGB(38, 38, 38));
        auto buttonArea = area.reduced(5);
        auto centerX = buttonArea.getCentreX();
        muteButton.setBounds(centerX - buttonWidth - 10, buttonArea.getY()+40, buttonWidth, buttonHeight);
        bypassButton.setBounds(centerX + 10, buttonArea.getY()+20, buttonWidth + 40, buttonHeight +40);

        // Debug statements
        std::cout << "MenuBar bounds: " << menuBar.getBounds().toString() << std::endl;
        std::cout << "MuteButton bounds: " << muteButton.getBounds().toString() << std::endl;
        std::cout << "BypassButton bounds: " << bypassButton.getBounds().toString() << std::endl;
    }

    // Menu bar related methods
    juce::StringArray getMenuBarNames() override
    {
        return {"File", "Edit", "Options"};
    }

    juce::PopupMenu getMenuForIndex(int /*menuIndex*/, const juce::String &menuName) override
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

    void menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) override
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
        updateMuteButton();
        if (muteCallback)
            muteCallback(muted);
    }

    void toggleBypass()
    {
        bypassed = !bypassed;
        updateBypassButton();
        if (bypassCallback)
            bypassCallback(bypassed);
    }

    void updateMuteButton()
    {
        auto muteSVG = juce::Drawable::createFromSVG(*juce::parseXML(muted ? SVGButtons::muteButtonOnSVG : SVGButtons::muteButtonOffSVG));
        muteButton.setImages(muteSVG.get());
    }

    void updateBypassButton()
    {
        auto bypassSVG = juce::Drawable::createFromSVG(*juce::parseXML(bypassed ? SVGButtons::bypassButtonOnSVG : SVGButtons::bypassButtonOffSVG));
        bypassButton.setImages(bypassSVG.get());
    }

    bool muted = false;
    bool bypassed = false;
    juce::String inputSource = " ";

    juce::DrawableButton muteButton { "Mute", juce::DrawableButton::ImageFitted };
    juce::DrawableButton bypassButton { "Bypass", juce::DrawableButton::ImageFitted };

    std::function<void(bool)> muteCallback = nullptr;
    std::function<void(bool)> bypassCallback = nullptr;

    juce::AudioDeviceManager& deviceManager;
    juce::ApplicationCommandManager* commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarComponent)
};