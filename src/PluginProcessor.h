#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include <vector>

constexpr int SCOPE_BUFFER_SIZE = 2048;

class CrokyScopeAudioProcessor  : public juce::AudioProcessor
{
public:
    CrokyScopeAudioProcessor();
    ~CrokyScopeAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "CrokyScope"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override { return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // APVTS
    juce::AudioProcessorValueTreeState treeState;
    
    // Lock-free buffer for HUD
    std::atomic<float> scopeData[SCOPE_BUFFER_SIZE];
    std::atomic<int> writePos {0};

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // For downsampling
    int samplesPerPixel = 128;
    int samplesSinceLastPixel = 0;
    float currentPeak = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrokyScopeAudioProcessor)
};
