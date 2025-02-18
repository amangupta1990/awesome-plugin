#define JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP 1
#include <JuceHeader.h>
#include "MainComponent.h"

class MainApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "GUI Application"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String &) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        std::cout << "Shutdown method called" << std::endl;
        if (mainWindow != nullptr)
        {
            auto* mainComponent = dynamic_cast<MainComponent*>(mainWindow->getContentComponent());
            if (mainComponent != nullptr)
            {
                juce::File pluginStateFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("pluginChainState.xml");
                std::cout << "Saving plugin chain state to: " << pluginStateFile.getFullPathName() << std::endl;
                mainComponent->savePluginChain(pluginStateFile);
            }
        }

        mainWindow = nullptr; // (deletes our window)
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String &) override {}

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name) : DocumentWindow(name,
                                                       juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
                                                       DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(MainApp)