#pragma once

#include <JuceHeader.h>
#include "ScopeComponent.h"

class CrokyScopyAudioProcessor;

namespace CrokyScopy
{

class HUDWindow : public juce::DocumentWindow
{
public:
    explicit HUDWindow(CrokyScopyAudioProcessor& p);
    ~HUDWindow() override;

    void closeButtonPressed() override;
    void resized() override;
    void moved() override;
    
    // Updates transparency based on APVTS
    void updateAppearance(float opacity);

private:
    CrokyScopyAudioProcessor& processor;
    std::unique_ptr<ScopeComponent> scopeComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HUDWindow)
};

} // namespace CrokyScopy
