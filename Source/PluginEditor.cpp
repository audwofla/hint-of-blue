/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HintofblueAudioProcessorEditor::HintofblueAudioProcessorEditor (HintofblueAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 300);

	inputGainSlider.setSliderStyle(juce::Slider::Rotary);
	inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(inputGainSlider);


    driveSlider.setSliderStyle(juce::Slider::Rotary);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(driveSlider);

	biasSlider.setSliderStyle(juce::Slider::Rotary);
	biasSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
	addAndMakeVisible(biasSlider);

    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "drive", driveSlider
    );
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "inputGainDb", inputGainSlider
    );
    biasAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "bias", biasSlider
	);

    inputGainLabel.setText("Input", juce::dontSendNotification);
    inputGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(inputGainLabel);

    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(driveLabel);

    biasLabel.setText("Bias", juce::dontSendNotification);
    biasLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(biasLabel);

}

HintofblueAudioProcessorEditor::~HintofblueAudioProcessorEditor()
{
}

//==============================================================================
void HintofblueAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("hint of blue", getLocalBounds(), juce::Justification::centred, 1);
}

void HintofblueAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds().reduced(40);

    auto colWidth = area.getWidth() / 3;

    auto col1 = area.removeFromLeft(colWidth);
    auto col2 = area.removeFromLeft(colWidth);
    auto col3 = area;

    inputGainSlider.setBounds(col1.removeFromTop(200));
    inputGainLabel.setBounds(col1);

    driveSlider.setBounds(col2.removeFromTop(200));
    driveLabel.setBounds(col2);

    biasSlider.setBounds(col3.removeFromTop(200));
    biasLabel.setBounds(col3);
}