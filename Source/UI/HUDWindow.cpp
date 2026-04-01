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
    
    // Safety check for Win32 attributes (sometimes handle changes)
    if (firstRunSizeForce && getWindowHandle() != nullptr)
    {
        setWindowAttributes();
        firstRunSizeForce = false;
    }
}

void HUDWindow::updateAppearance()
{
    float newOpacity = processor.apvts.getRawParameterValue("opacity")->load();
    bool newEditMode = processor.apvts.getRawParameterValue("edit_mode")->load() > 0.5f;

    if (newOpacity != currentOpacity || newEditMode != currentlyInEditMode || !attributesApplied)
    {
        currentOpacity = newOpacity;
        currentlyInEditMode = newEditMode;
        setAlpha(currentOpacity);
        setWindowAttributes();
    }
}

void HUDWindow::setWindowAttributes()
{
#if JUCE_WINDOWS
    HWND hwnd = (HWND)getWindowHandle();
    if (hwnd != nullptr)
    {
        // 1. Basic Style (Minimize box/Menu)
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        style |= WS_MINIMIZEBOX | WS_SYSMENU;
        SetWindowLong(hwnd, GWL_STYLE, style);

        // 2. Extended Style (Layered, Toolwindow)
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_TOOLWINDOW | WS_EX_LAYERED;

        // Toggle Click-through if not in Edit Mode
        if (!currentlyInEditMode)
            exStyle |= WS_EX_TRANSPARENT;
        else
            exStyle &= ~WS_EX_TRANSPARENT;

        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
        
        // 3. Final Attributes
        SetLayeredWindowAttributes(hwnd, 0, 255, 0x00000002); // LWA_ALPHA
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, 
                     SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOACTIVATE);
        
        attributesApplied = true;
    }
#endif
}

} // namespace CrokyScopy
