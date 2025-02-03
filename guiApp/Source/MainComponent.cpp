// filepath: /Users/amangupta/projects/awesome-plugin/plugin/Source/MainComponent.cpp
#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(800, 600);
}

MainComponent::~MainComponent() {}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::black);
    g.setFont(20.0f);
    g.drawText("Hello, JUCE!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized() {}