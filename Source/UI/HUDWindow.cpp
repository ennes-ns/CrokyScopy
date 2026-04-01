#include "HUDWindow.h"
#include "../DSP/PluginProcessor.h"

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

    // --- Position Restoration ---
    float savedX = *p.apvts.getRawParameterValue("hud_x");
    float savedY = *p.apvts.getRawParameterValue("hud_y");
    int savedW = (int)*p.apvts.getRawParameterValue("hud_width");
    int savedH = (int)*p.apvts.getRawParameterValue("hud_height");

    // Safety defaults
    if (savedW <= 20) savedW = 800;
    if (savedH <= 10) savedH = 300;

    if (savedX >= 0 && savedY >= 0)
        setBounds((int)savedX, (int)savedY, savedW, savedH);
    else
        centreWithSize(savedW, savedH);

    // --- Content ---
    scopeComponent = std::make_unique<ScopeComponent>(processor);
    scopeComponent->setVisible(true);
    setContentOwned(scopeComponent.get(), true);
    setBackgroundColour(juce::Colours::transparentBlack);
    
    // CRITICAL: Ensure top-level visibility in a plugin host
    addToDesktop(0);
    
    startTimerHz(30); // 30Hz for parameter syncing
}

HUDWindow::~HUDWindow()
{
    stopTimer();
    clearContentComponent();
}

void HUDWindow::closeButtonPressed()
{
    processor.apvts.getRawParameterValue("show_hud")->store(0.0f);
    processor.toggleHUD(false);
}

void HUDWindow::resized()
{
    if (scopeComponent != nullptr)
        scopeComponent->setBounds(getLocalBounds());

    // Persist size back to APVTS
    processor.apvts.getRawParameterValue("hud_width")->store((float)getWidth());
    processor.apvts.getRawParameterValue("hud_height")->store((float)getHeight());
}

void HUDWindow::moved()
{
    // Persist position back to APVTS
    processor.apvts.getRawParameterValue("hud_x")->store((float)getX());
    processor.apvts.getRawParameterValue("hud_y")->store((float)getY());
}

void HUDWindow::timerCallback()
{
    updateAppearance();
}

void HUDWindow::updateAppearance()
{
    // --- State Fetching ---
    auto opacityParam = processor.apvts.getRawParameterValue("opacity");
    auto editModeParam = processor.apvts.getRawParameterValue("edit_mode");
    
    if (opacityParam == nullptr || editModeParam == nullptr) return;

    float currentOpacity = opacityParam->load();
    bool currentEditMode = editModeParam->load() > 0.5f;

    // --- Change-Only Logic (Eliminates PC Lag) ---
    // We only call expensive Win32/JUCE state functions if the parameters actually change.
    if (firstRun || currentOpacity != lastOpacity || currentEditMode != lastEditMode)
    {
        setAlpha(currentOpacity);
        setWindowAttributes(); // Only called on change!
        
        lastOpacity = currentOpacity;
        lastEditMode = currentEditMode;
        firstRun = false;
    }
}

void HUDWindow::setWindowAttributes()
{
#if JUCE_WINDOWS
    HWND hwnd = (HWND)getWindowHandle();
    if (hwnd != nullptr)
    {
        // 1. Extended Style (Layered, Toolwindow)
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        
        // Ensure Layered and Toolwindow are always set
        exStyle |= WS_EX_TOOLWINDOW | WS_EX_LAYERED;

        // Toggle Click-through based on cached lastEditMode
        if (!lastEditMode)
            exStyle |= WS_EX_TRANSPARENT;
        else
            exStyle &= ~WS_EX_TRANSPARENT;

        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
        
        // 2. Final Attributes (Zero Hammering)
        // LWA_ALPHA logic
        SetLayeredWindowAttributes(hwnd, 0, 255, 0x00000002);
        
        // Force topmost without triggering expensive frame changes repeatedly
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, 
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
#endif
}

} // namespace CrokyScopy
