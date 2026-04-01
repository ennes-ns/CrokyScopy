#include "PluginProcessor.h"
#include "../UI/HUDWindow.h"

CrokyScopyAudioProcessor::CrokyScopyAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Start the Safe UI Lifecycle Manager Timer
    // 10 Hz is plenty to check if the user clicked "Show HUD"
    startTimerHz(10);
}

CrokyScopyAudioProcessor::~CrokyScopyAudioProcessor()
{
    stopTimer();
    // Ensure HUD is explicitly deleted before processor dies
    hudWindow.reset(); 
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
    // NO UI CALLS HERE. ZERO LAG GUARANTEE.

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // --- State Check ---
    bool isPaused = apvts.getRawParameterValue("paused")->load() > 0.5f;
    if (isPaused) return;

    // --- Transport Dynamics ---
    double bpm = 120.0;
    double ppq = 0.0;
    bool isPlaying = true;

    if (auto* ph = getPlayHead())
    {
        if (auto pos = ph->getPosition())
        {
            if (auto p_bpm = pos->getBpm()) bpm = *p_bpm;
            if (auto p_ppq = pos->getPpqPosition()) ppq = *p_ppq;
            isPlaying = pos->getIsPlaying();
        }
    }

    bool transportSync = apvts.getRawParameterValue("transport_sync")->load() > 0.5f;
    if (transportSync && !isPlaying) return;

    // --- Data Capture ---
    float beatIdx = apvts.getRawParameterValue("beats")->load();
    double beatsPerPass = (beatIdx < 0.5f) ? 2.0 : ((beatIdx < 1.5f) ? 4.0 : ((beatIdx < 2.5f) ? 8.0 : 16.0));

    if (totalNumInputChannels > 0)
        scopeBuffer.pushBlock(buffer, totalNumInputChannels, ppq, bpm, beatsPerPass);
}

// ==============================================================================
// Safe UI Lifecycle Manager (GUI THREAD)
// ==============================================================================
void CrokyScopyAudioProcessor::timerCallback()
{
    bool shouldShow = apvts.getRawParameterValue("show_hud")->load() > 0.5f;
    
    // Toggle Lifecycle
    if (shouldShow && hudWindow == nullptr)
    {
        // Add to desktop and display
        hudWindow = std::make_unique<CrokyScopy::HUDWindow>(*this);
        hudWindow->setVisible(true);
        hudWindow->toFront(true);
    }
    else if (!shouldShow && hudWindow != nullptr)
    {
        // Graceful teardown
        hudWindow.reset();
    }

    // Appearance Polling (Change-Only)
    if (hudWindow != nullptr)
    {
        float curOpacity = apvts.getRawParameterValue("opacity")->load();
        if (curOpacity != lastOpacity)
        {
            hudWindow->updateAppearance(curOpacity);
            lastOpacity = curOpacity;
        }
    }
}

juce::AudioProcessorEditor* CrokyScopyAudioProcessor::createEditor()
{
    // Bitwig default UI
    return new juce::GenericAudioProcessorEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout CrokyScopyAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("opacity", "HUD Opacity", 0.0f, 1.0f, 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("line_width", "Line Width", 0.5f, 10.0f, 2.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("hue", "Wave Color (Hue)", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("vertical_zoom", "Vertical Zoom", 0.1f, 10.0f, 1.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>("beats", "Time Scale", juce::StringArray{"2 Beats", "4 Beats", "8 Beats", "16 Beats"}, 1));
    layout.add(std::make_unique<juce::AudioParameterBool>("paused", "Pause Visualization", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("transport_sync", "Sync to Transport", false));

    layout.add(std::make_unique<juce::AudioParameterBool>("show_hud", "Show HUD", true));
    
    // Position Persistence
    layout.add(std::make_unique<juce::AudioParameterFloat>("hud_x", "HUD X", -1.0f, 5000.0f, -1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("hud_y", "HUD Y", -1.0f, 5000.0f, -1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("hud_width", "HUD Width", 20.0f, 5000.0f, 800.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("hud_height", "HUD Height", 10.0f, 5000.0f, 300.0f));

    return layout;
}

void CrokyScopyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CrokyScopyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// Entry Point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new CrokyScopyAudioProcessor(); }
