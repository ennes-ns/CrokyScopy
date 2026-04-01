#pragma once

#include <JuceHeader.h>
#include "Common.h"

// Forward declaration
class CrokyScopyAudioProcessor;

namespace CrokyScopy
{

/**
 * @class ScopeComponent
 * @brief The main graphical component for the cycling Oscillogram.
 * 
 * Runs on a juce::Timer to redraw the ScopeBuffer state independently
 * of the main DAW UI. Uses offscreen caching for static grids.
 */
class ScopeComponent : public juce::Component, public juce::Timer
{
public:
    explicit ScopeComponent(CrokyScopyAudioProcessor& p);
    ~ScopeComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    void renderGridToCache();
    void renderWaveform(juce::Graphics& g);
    void updateVisualsFromAPVTS();
    
    CrokyScopyAudioProcessor& processor;

    juce::Image gridCache;
    Palette currentPalette;
    VisualSettings currentSettings;

    float currentBeats { 4.0f };
    bool inEditMode { false };

    // Edit Mode Sub-components
    juce::ComponentDragger dragger;
    std::unique_ptr<juce::ResizableCornerComponent> resizer;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeComponent)
};

} // namespace CrokyScopy

