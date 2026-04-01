#pragma once

#include <JuceHeader.h>
#include "../Core/ScopeBuffer.h"

class CrokyScopyAudioProcessor;

namespace CrokyScopy
{

class ScopeComponent : public juce::Component, public juce::Timer
{
public:
    explicit ScopeComponent(CrokyScopyAudioProcessor& p);
    ~ScopeComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    CrokyScopyAudioProcessor& processor;
    
    // UI Caching to prevent allocation in paint()
    juce::Path wavePath; 
    juce::ComponentDragger dragger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeComponent)
};

} // namespace CrokyScopy
