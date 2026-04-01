#pragma once

#include <JuceHeader.h>

class CrokyScopyAudioProcessor;

namespace CrokyScopy
{

/**
 * @class ScopeComponent
 * @brief The actual painting surface for the oscilloscope.
 */
class ScopeComponent : public juce::Component, public juce::Timer
{
public:
    explicit ScopeComponent(CrokyScopyAudioProcessor& p);
    ~ScopeComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    // --- Mouse Handlers for Move/Resize ---
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    void renderGridToCache();
    void renderWaveform(juce::Graphics& g);

    CrokyScopyAudioProcessor& processor;

    // Rendering Cache
    juce::Image gridCache;
    bool needsGridRepaint { true };

    // Performance State
    int lastWriteIndex { 0 };
    float currentHue { 0.5f };
    float currentLineWidth { 2.0f };
    float currentVerticalZoom { 1.0f };

    // Interaction State
    juce::ComponentBoundsConstrainer resizeConstrainer;
    juce::ComponentDragger dragger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeComponent)
};

} // namespace CrokyScopy
