#include "PluginProcessor.h"
#include "PluginEditor.h"

CrokyScopyAudioProcessor::CrokyScopyAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

CrokyScopyAudioProcessor::~CrokyScopyAudioProcessor()
{
}

void CrokyScopyAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    scopeBuffer.prepare(sampleRate);
}

void CrokyScopyAudioProcessor::releaseResources()
{
}

bool CrokyScopyAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void CrokyScopyAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Retrieve beats per pass from APVTS (Index 0=4, 1=8, 2=16)
    float beatIdx = apvts.getRawParameterValue("beats")->load();
    double beatsPerPass = (beatIdx < 0.5f) ? 4.0 : ((beatIdx < 1.5f) ? 8.0 : 16.0);

    // Sync with DAW Playhead
    if (auto* playHead = getPlayHead())
    {
        if (auto posInfo = playHead->getPosition())
        {
            if (posInfo->getIsPlaying())
            {
                double ppq = posInfo->getPpqPosition().orFallback(0.0);
                double bpm = posInfo->getBpm().orFallback(120.0);
                
                scopeBuffer.pushBlock(buffer, totalNumInputChannels, ppq, bpm, beatsPerPass);
            }
        }
    }
}

juce::AudioProcessorEditor* CrokyScopyAudioProcessor::createEditor()
{
    return new CrokyScopyAudioProcessorEditor(*this);
}

void CrokyScopyAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CrokyScopyAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void CrokyScopyAudioProcessor::toggleHUD(bool shouldBeOpen)
{
    if (shouldBeOpen)
    {
        if (hudWindow == nullptr)
        {
            hudWindow = std::make_unique<HUDWindow>("CrokyScopy HUD", juce::Colours::transparentBlack, 0);
            hudWindow->setVisible(true);
        }
    }
    else
    {
        hudWindow.reset();
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout CrokyScopyAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "opacity", 1 }, "HUD Opacity", 0.0f, 1.0f, 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "line_width", 1 }, "Line Width", 0.5f, 10.0f, 2.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "beats", 1 }, "Beats per Pass", juce::StringArray{"4 Beats", "8 Beats", "16 Beats"}, 0));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "edit_mode", 1 }, "Edit Tool", false));
    
    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrokyScopyAudioProcessor();
}

