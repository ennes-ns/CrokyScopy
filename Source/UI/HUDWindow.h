#pragma once

#include <JuceHeader.h>
#include "ScopeComponent.h"

class CrokyScopyAudioProcessor;

namespace CrokyScopy
{

/**
 * @class HUDWindow
 * @brief The floating, borderless top-level window (Win32 specific).
 */
class HUDWindow : public juce::DocumentWindow, public juce::Timer
{
public:
    explicit HUDWindow(CrokyScopyAudioProcessor& p);
    ~HUDWindow() override;

    void closeButtonPressed() override;
    void resized() override;
    void moved() override;
    void timerCallback() override;

private:
    void setWindowAttributes();
    void updateAppearance();

    CrokyScopyAudioProcessor& processor;
    std::unique_ptr<ScopeComponent> scopeComponent;
    
    // Performance State Caching (Fixes PC Lag)
    bool lastEditMode { false };
    float lastOpacity { -1.0f };
    bool firstRun { true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HUDWindow)
};

} // namespace CrokyScopy
