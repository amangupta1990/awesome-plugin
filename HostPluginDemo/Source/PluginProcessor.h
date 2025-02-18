#pragma once

#include <JuceHeader.h>

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor()
    {
        // ...existing code...
    }

    ~PluginProcessor() override
    {
        // ...existing code...
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        // ...existing code...
    }

    void releaseResources() override
    {
        // ...existing code...
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        // ...existing code...
    }

    juce::AudioProcessorEditor* createEditor() override
    {
        // ...existing code...
    }

    bool hasEditor() const override
    {
        // ...existing code...
    }

    const juce::String getName() const override
    {
        // ...existing code...
    }

    bool acceptsMidi() const override
    {
        // ...existing code...
    }

    bool producesMidi() const override
    {
        // ...existing code...
    }

    bool isMidiEffect() const override
    {
        // ...existing code...
    }

    double getTailLengthSeconds() const override
    {
        // ...existing code...
    }

    int getNumPrograms() override
    {
        // ...existing code...
    }

    int getCurrentProgram() override
    {
        // ...existing code...
    }

    void setCurrentProgram(int index) override
    {
        // ...existing code...
    }

    const juce::String getProgramName(int index) override
    {
        // ...existing code...
    }

    void changeProgramName(int index, const juce::String& newName) override
    {
        // ...existing code...
    }

    void getStateInformation(juce::MemoryBlock& destData) override
    {
        std::cout << "getStateInformation called" << std::endl;
        juce::MemoryOutputStream stream(destData, true);
        // Save your plugin's state to the stream
        // For example:
        // stream.writeFloat(*gainParameter);
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
        std::cout << "setStateInformation called" << std::endl;
        juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
        // Load your plugin's state from the stream
        // For example:
        // *gainParameter = stream.readFloat();
    }

private:
    // ...existing code...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
