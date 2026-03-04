/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class HintofblueAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    HintofblueAudioProcessorEditor (HintofblueAudioProcessor&);
    ~HintofblueAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HintofblueAudioProcessor& audioProcessor;

	juce::Slider inputGainSlider;
    juce::Slider driveSlider;
	juce::Slider biasSlider;

    juce::Label inputGainLabel;
	juce::Label driveLabel;
    juce::Label biasLabel;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> biasAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HintofblueAudioProcessorEditor)
};
