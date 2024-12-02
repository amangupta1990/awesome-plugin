#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // Configure the midiVolume slider
    midiVolume.setSliderStyle (juce::Slider::LinearBarVertical);
    midiVolume.setRange (0.0, 127.0 , 1.0); // MIDI volumes typically range from 0 to 127
    midiVolume.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    midiVolume.setPopupDisplayEnabled(true, false, this);
    midiVolume.setTextValueSuffix (" Volume");
    midiVolume.setValue(64.0); // Set a midpoint default value for MIDI volume

    addAndMakeVisible (midiVolume);
    midiVolume.addListener (this);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    midiVolume.removeListener(this);
}

void AudioPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &midiVolume)
    {
        // Handle the slider value change
        processorRef.setMidiVolume(midiVolume.getValue());
    }
}