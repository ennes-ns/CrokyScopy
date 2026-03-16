#pragma once

#include <JuceHeader.h>

class HUDWindow : public juce::DocumentWindow
{
public:
    HUDWindow(const juce::String& name, juce::Colour backgroundColour, int buttonsNeeded);
    ~HUDWindow() override;

    void closeButtonPressed() override;

    // Win32 Hack for transparency and taskbar hiding
#if JUCE_WINDOWS
    void setWindowAttributes();
#endif

private:
    juce::ComponentDragger dragger;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HUDWindow)
};
