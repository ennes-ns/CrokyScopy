#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class CrokyScopeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    CrokyScopeAudioProcessorEditor(CrokyScopeAudioProcessor&);
    ~CrokyScopeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    CrokyScopeAudioProcessor& audioProcessor;
    
    juce::TextButton hudToggleButton { "Toggle HUD" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrokyScopeAudioProcessorEditor)
};
