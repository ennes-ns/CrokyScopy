#pragma once

#include <JuceHeader.h>
#include "ScopeComponent.h"

class CrokyScopyAudioProcessor;

namespace CrokyScopy
{

/**
 * @class HUDWindow
 * @brief The floating, borderless top-level window.
 * 
 * Uses Win32-specific hacks to achieve a true Wave Candy style HUD. 
 * Periodically checks APVTS for opacity and Edit Mode toggles.
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
    
    bool currentlyInEditMode { false };
    float currentOpacity { 1.0f };

    int lastSyncMode { -1 };
    int lastDrawMode { -1 };
    int newSyncMode { 0 };
    int newDrawMode { 0 };
    bool nativeStyleChanged { true };
    bool attributesApplied { false };
    bool firstRunSizeForce { true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HUDWindow)
};

} // namespace CrokyScopy
