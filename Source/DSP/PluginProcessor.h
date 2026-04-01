#pragma once

#include <JuceHeader.h>
#include "../Core/ScopeBuffer.h"

namespace CrokyScopy { class HUDWindow; }

class CrokyScopyAudioProcessor : public juce::AudioProcessor, private juce::Timer
{
public:
    CrokyScopyAudioProcessor();
    ~CrokyScopyAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    const CrokyScopy::ScopeBuffer& getScopeBuffer() const { return scopeBuffer; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // The Safe GUI Timer
    void timerCallback() override;

    CrokyScopy::ScopeBuffer scopeBuffer;
    std::unique_ptr<CrokyScopy::HUDWindow> hudWindow;
    
    float lastOpacity = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrokyScopyAudioProcessor)
};
