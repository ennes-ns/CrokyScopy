#include "PluginProcessor.h"
#include "PluginEditor.h"

CrokyScopeAudioProcessor::CrokyScopeAudioProcessor()
     : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     ),
       treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    for (int i = 0; i < SCOPE_BUFFER_SIZE; ++i)
        scopeData[i].store(0.0f);
}

CrokyScopeAudioProcessor::~CrokyScopeAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout CrokyScopeAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("show_hud", 1), "Show HUD", true));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("opacity", 1), "Opacity", 0.1f, 1.0f, 0.8f));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("line_width", 1), "Line Width", 0.5f, 10.0f, 2.0f));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("hue", 1), "Color Hue", 0.0f, 1.0f, 0.4f)); // green-ish
        
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("beats", 1), "Beats Displayed", juce::StringArray{"1", "2", "4", "8", "16"}, 2)); // 4 beats default
        
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("paused", 1), "Paused", false));
        
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("transport_sync", 1), "Transport Sync", true));

    // Hidden float params for rect layout. We can use floats and cast them to ints since they don't need real UI bounds usually.
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hud_x", 1), "HUD X", -5000.0f, 5000.0f, 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hud_y", 1), "HUD Y", -5000.0f, 5000.0f, 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hud_w", 1), "HUD Width", 100.0f, 4000.0f, 400.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hud_h", 1), "HUD Height", 100.0f, 4000.0f, 200.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("edit_mode", 1), "Edit Mode", false));

    return { params.begin(), params.end() };
}

void CrokyScopeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // A simplified downsampling heuristic
    samplesPerPixel = static_cast<int>(sampleRate / 480.0); 
    if (samplesPerPixel < 1) samplesPerPixel = 1;
    samplesSinceLastPixel = 0;
    currentPeak = 0.0f;
}

void CrokyScopeAudioProcessor::releaseResources()
{
}

void CrokyScopeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    
    // Check if paused
    auto* pausedParam = treeState.getRawParameterValue("paused");
    auto* transportSyncParam = treeState.getRawParameterValue("transport_sync");
    
    bool shouldPause = (pausedParam && pausedParam->load() > 0.5f);
    
    // Transport sync logic
    if (transportSyncParam && transportSyncParam->load() > 0.5f) {
        if (auto* ph = getPlayHead()) {
            if (auto pos = ph->getPosition()) {
                if (!pos->getIsPlaying()) {
                    shouldPause = true;
                }
                
                // If we want exact beat matching, we'd dynamically adjust samplesPerPixel here.
                // For now, simpler RMS/peak flow
            }
        }
    }

    if (!shouldPause) {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float maxAbs = 0.0f;
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                float s = std::abs(buffer.getSample(channel, sample));
                if (s > maxAbs) maxAbs = s;
            }
            
            if (maxAbs > currentPeak) currentPeak = maxAbs;
            
            samplesSinceLastPixel++;
            if (samplesSinceLastPixel >= samplesPerPixel) {
                // write to circular buffer
                int pos = writePos.load(std::memory_order_relaxed);
                scopeData[pos].store(currentPeak, std::memory_order_relaxed);
                writePos.store((pos + 1) % SCOPE_BUFFER_SIZE, std::memory_order_release);
                
                samplesSinceLastPixel = 0;
                currentPeak = 0.0f;
            }
        }
    }

    // Clear output (this is an analysis tool, maybe bypass is better in DAW, but clear unused channels to be safe)
    for (int i = totalNumInputChannels; i < getTotalNumOutputChannels(); ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

juce::AudioProcessorEditor* CrokyScopeAudioProcessor::createEditor()
{
    return new CrokyScopeAudioProcessorEditor (*this);
}

void CrokyScopeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CrokyScopeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (treeState.state.getType()))
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrokyScopeAudioProcessor();
}
