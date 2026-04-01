#include "HUDWindow.h"
#include "PluginProcessor.h"

#if JUCE_WINDOWS
#include <Windows.h>
#endif

namespace CrokyScopy
{

HUDWindow::HUDWindow(CrokyScopyAudioProcessor& p)
    : DocumentWindow("CrokyScopy HUD", juce::Colours::transparentBlack, 0),
      processor(p)
{
    setUsingNativeTitleBar(false);
    setAlwaysOnTop(true);
    setResizable(true, true);

    // Default sizing
    setSize(800, 300);

    scopeComponent = std::make_unique<ScopeComponent>(processor);
    setContentNonOwned(scopeComponent.get(), true);
    
    // Poll for APVTS changes
    startTimerHz(15);
}

HUDWindow::~HUDWindow()
{
    stopTimer();
    clearContentComponent();
}

void HUDWindow::closeButtonPressed()
{
    // The user pressed Alt+F4 or something similar. 
    // Usually a HUD shouldn't close itself unless the editor toggles it.
}

void HUDWindow::timerCallback()
{
    updateAppearance();
}

void HUDWindow::updateAppearance()
{
    float newOpacity = processor.apvts.getRawParameterValue("opacity")->load();
    bool newEditMode = processor.apvts.getRawParameterValue("edit_mode")->load() > 0.5f;

    bool appearanceChanged = false;

    if (newOpacity != currentOpacity)
    {
        currentOpacity = newOpacity;
        setAlpha(currentOpacity);
        appearanceChanged = true;
    }

    if (newEditMode != currentlyInEditMode)
    {
        currentlyInEditMode = newEditMode;
        appearanceChanged = true;
    }

    if (appearanceChanged)
        setWindowAttributes(); // Reapply Win32 hacks if modes change
}

void HUDWindow::setWindowAttributes()
{
#if JUCE_WINDOWS
    HWND hwnd = (HWND)getWindowHandle();
    if (hwnd != nullptr)
    {
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        
        // Always layered and toolwindow (hides from taskbar)
        exStyle |= WS_EX_LAYERED | WS_EX_TOOLWINDOW;

        // If not in edit mode, make it fully click-through
        if (!currentlyInEditMode)
            exStyle |= WS_EX_TRANSPARENT;
        else
            exStyle &= ~WS_EX_TRANSPARENT;

        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
    }
#endif
}

} // namespace CrokyScopy
