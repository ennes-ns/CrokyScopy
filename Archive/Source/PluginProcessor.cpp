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

juce::AudioProcessorEditor* CrokyScopyAudioProcessor::createEditor()
{
    return new CrokyScopyAudioProcessorEditor(*this);
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

    // --- HUD LOGIC ---
    bool isPaused = apvts.getRawParameterValue("paused")->load() > 0.5f;
    bool isTransportSync = apvts.getRawParameterValue("transport_sync")->load() > 0.5f;
    
    if (isPaused) return;

    if (isTransportSync)
    {
        if (auto* ph = getPlayHead())
        {
            if (auto pos = ph->getPosition())
            {
                if (!pos->getIsPlaying()) return;
            }
        }
    }

    // Capture to scope buffer
    float beats = 4.0f;
    int beatIdx = (int)apvts.getRawParameterValue("beats")->load();
    if (beatIdx == 0) beats = 2.0f;
    else if (beatIdx == 1) beats = 4.0f;
    else if (beatIdx == 2) beats = 8.0f;
    else if (beatIdx == 3) beats = 16.0f;

    double bpm = 120.0;
    double ppq = 0.0;
    
    if (auto* ph = getPlayHead())
    {
        if (auto pos = ph->getPosition())
        {
            if (auto p_bpm = pos->getBpm()) bpm = *p_bpm;
            if (auto p_ppq = pos->getPpqPosition()) ppq = *p_ppq;
        }
    }
    }

    if (totalNumInputChannels > 0)
        scopeBuffer.pushBlock(buffer, totalNumInputChannels, ppq, bpm, (double)beats);
}

void CrokyScopyAudioProcessor::toggleHUD(bool show)
{
    if (show)
    {
        if (hudWindow == nullptr)
        {
            hudWindow = std::make_unique<CrokyScopy::HUDWindow>(*this);
        }
        hudWindow->setVisible(true);
        hudWindow->toFront(false);
    }
    else
    {
        hudWindow.reset();
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout CrokyScopyAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("opacity", "HUD Opacity", 0.0f, 1.0f, 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("line_width", "Line Width", 0.5f, 10.0f, 2.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("beats", "Time (Beats)", juce::StringArray{"2 Beats", "4 Beats", "8 Beats", "16 Beats"}, 1));
    layout.add(std::make_unique<juce::AudioParameterChoice>("sync_mode", "Sync Mode", juce::StringArray{"Synced", "Free"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("free_time", "Cycle Time (Seconds)", juce::NormalisableRange<float>(0.01f, 5.0f, 0.01f, 0.5f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("draw_mode", "Draw Mode", juce::StringArray{"Paging", "Scrolling"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("hue", "Line Color (Hue)", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("vertical_zoom", "Vertical Zoom", 0.1f, 10.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("paused", "Pause Scope", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("transport_sync", "Transport Sync", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("show_hud", "Show HUD", true));
    layout.add(std::make_unique<juce::AudioParameterBool>("edit_mode", "HUD Edit Mode", true));
    
    // Position Persistence
    layout.add(std::make_unique<juce::AudioParameterFloat>("hud_x", "HUD X", -1.0f, 4000.0f, -1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("hud_y", "HUD Y", -1.0f, 4000.0f, -1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("hud_width", "HUD Width", 20.0f, 4000.0f, 800.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("hud_height", "HUD Height", 10.0f, 4000.0f, 300.0f));
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
    {
        if (xmlState->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
            if (apvts.getRawParameterValue("show_hud")->load() > 0.5f) {
                juce::MessageManager::callAsync([this]() { toggleHUD(true); });
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new CrokyScopyAudioProcessor(); }
