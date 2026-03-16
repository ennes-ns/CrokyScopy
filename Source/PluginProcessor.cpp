#include "PluginProcessor.h"
#include "PluginEditor.h"

CrokyScopeAudioProcessor::CrokyScopeAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

CrokyScopeAudioProcessor::~CrokyScopeAudioProcessor()
{
}

void CrokyScopeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void CrokyScopeAudioProcessor::releaseResources()
{
}

bool CrokyScopeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void CrokyScopeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Simple Envelope Follower logic (to be refined)
    float maxLevel = 0.0f;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float level = buffer.getMagnitude(channel, 0, buffer.getNumSamples());
        if (level > maxLevel) maxLevel = level;
    }
    
    meterValue.set(maxLevel);
}

juce::AudioProcessorEditor* CrokyScopeAudioProcessor::createEditor()
{
    return new CrokyScopeAudioProcessorEditor(*this);
}

void CrokyScopeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CrokyScopeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void CrokyScopeAudioProcessor::toggleHUD(bool shouldBeOpen)
{
    if (shouldBeOpen)
    {
        if (hudWindow == nullptr)
        {
            hudWindow = std::make_unique<HUDWindow>("CrokyScope HUD", juce::Colours::transparentBlack, 0);
            hudWindow->setVisible(true);
        }
    }
    else
    {
        hudWindow.reset();
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout CrokyScopeAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "opacity", 1 }, "Opacity", 0.0f, 1.0f, 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "scale", 1 }, "Scale", 0.5f, 2.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "smoothing", 1 }, "Smoothing", 0.0f, 1.0f, 0.5f));
    
    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrokyScopeAudioProcessor();
}
