#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    // Configure the midiVolume slider
    midiVolumeSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    midiVolumeSlider.setRange(0.0, 127.0, 1.0); // MIDI volumes typically range from 0 to 127
    midiVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    midiVolumeSlider.setPopupDisplayEnabled(true, false, this);
    midiVolumeSlider.setTextValueSuffix(" Volume");
    midiVolumeSlider.setValue(64.0); // Set a midpoint default value for MIDI volume

    addAndMakeVisible(midiVolumeSlider);
    midiVolumeSlider.addListener(this);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    midiVolumeSlider.removeListener(this);
}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // Fill the background with a solid colour
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Set the colour and font for drawing text
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    // Draw some text in the centre of the component
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Lay out the positions of any subcomponents in your editor
    midiVolumeSlider.setBounds(getLocalBounds().reduced(40));
}

void AudioPluginAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &midiVolumeSlider)
    {
        // Handle the slider value change
        processorRef.setMidiVolume(midiVolumeSlider.getValue());
    }
}