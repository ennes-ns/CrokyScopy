#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class ScopeHUDWindow : public juce::TopLevelWindow,
                       public juce::Timer
{
public:
    ScopeHUDWindow(CrokyScopeAudioProcessor& p);
    ~ScopeHUDWindow() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    // Mouse events for draggability in edit mode
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

private:
    CrokyScopeAudioProcessor& processor;
    juce::ComponentDragger dragger;
    juce::Rectangle<int> startBounds;
    
    // Internal cache for rendering
    std::vector<float> localData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeHUDWindow)
};
