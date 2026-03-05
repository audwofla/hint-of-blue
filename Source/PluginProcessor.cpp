/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HintofblueAudioProcessor::HintofblueAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
	, apvts(*this, nullptr, "Parameters", createParameters())
{
}

HintofblueAudioProcessor::~HintofblueAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
HintofblueAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "inputGainDb", 
        "Input Gain (dB)",    
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.01f),
        0.0f              
	));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "drive", 
        "Drive",    
        juce::NormalisableRange<float>(1.0f, 24.0f, 0.01f),
        1.0f              
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "driveType",
        "Drive Type",
        juce::StringArray{ "tanh", "atan" },
        0
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "bias", 
        "Bias",    
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f),
        0.0f              
	));

    return { params.begin(), params.end() };
}


const juce::String HintofblueAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HintofblueAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HintofblueAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HintofblueAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HintofblueAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HintofblueAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HintofblueAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HintofblueAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HintofblueAudioProcessor::getProgramName (int index)
{
    return {};
}

void HintofblueAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HintofblueAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    oversampling.reset();
    oversampling.initProcessing((size_t)samplesPerBlock);

    setLatencySamples((int)oversampling.getLatencyInSamples());

}
void HintofblueAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HintofblueAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HintofblueAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

	//auto autogainDb = apvts.getRawParameterValue("outputGainDb")->load();   
	//auto gainLin = juce::Decibels::decibelsToGain(autogainDb);
	//buffer.applyGain(gainLin);
    // 
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
	auto inputGainDb = apvts.getRawParameterValue("inputGainDb")->load();
	auto inputGain = juce::Decibels::decibelsToGain(inputGainDb);

	auto driveType = (int)apvts.getRawParameterValue("driveType")->load();
    auto drive = apvts.getRawParameterValue("drive")->load();
    auto outTrim = 1.0f / std::sqrt(std::max(1.0f, drive));

	auto bias = apvts.getRawParameterValue("bias")->load();
    float biasCompTanh = std::tanh(bias);
	float biasCompAtan = std::atan(bias);

	juce::dsp::AudioBlock<float> block(buffer);
	auto osBlock = oversampling.processSamplesUp(block);

    const int osNumCh = (int)osBlock.getNumChannels();
    const int osNumSamp = (int)osBlock.getNumSamples();

    for (int ch = 0; ch < osNumCh; ++ch)
    {
        float* x = osBlock.getChannelPointer((size_t)ch);

        for (int i = 0; i < osNumSamp; ++i)
        {
            float v = x[i] * inputGain;
            v *= drive;

            float y;
            if (driveType == 0)
            {
                y = std::tanh(v + biasCompTanh) - biasCompTanh;
            } 
            else
            {
                float biasCompAtan = std::atan(bias);
				y = std::atan(v + biasCompAtan) - biasCompAtan;
            }

            x[i] = y * outTrim;
        }
    }

    oversampling.processSamplesDown(block);

}

//==============================================================================
bool HintofblueAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HintofblueAudioProcessor::createEditor()
{
    return new HintofblueAudioProcessorEditor (*this);
}

//==============================================================================
void HintofblueAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	auto state = apvts.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void HintofblueAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
			apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HintofblueAudioProcessor();
}
