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

    // Initial size & Position Restoration
    float savedX = *p.apvts.getRawParameterValue("hud_x");
    float savedY = *p.apvts.getRawParameterValue("hud_y");
    int savedW = (int)*p.apvts.getRawParameterValue("hud_width");
    int savedH = (int)*p.apvts.getRawParameterValue("hud_height");

    // Defaults if uninitialized
    if (savedW <= 0) savedW = 800;
    if (savedH <= 0) savedH = 300;

    if (savedX >= 0 && savedY >= 0)
    {
        setBounds((int)savedX, (int)savedY, savedW, savedH);
    }
    else
    {
        centreWithSize(savedW, savedH);
    }

    scopeComponent = std::make_unique<ScopeComponent>(processor);
    scopeComponent->setVisible(true); // CRITICAL: Component must be visible
    setContentOwned(scopeComponent.get(), true);
    setBackgroundColour(juce::Colours::transparentBlack);
    startTimerHz(15);
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
    // 1) Layout the child
    if (scopeComponent != nullptr)
        scopeComponent->setBounds(getLocalBounds());

    // 2) Persist size back to APVTS
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
    if (firstRunSizeForce && getWindowHandle() != nullptr)
    {
        // Re-read from APVTS or use safe default
        float w = processor.apvts.getRawParameterValue("hud_width")->load();
        float h = processor.apvts.getRawParameterValue("hud_height")->load();
        centreWithSize((int)w, (int)h);
        firstRunSizeForce = false;
    }
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

    if (newSyncMode != lastSyncMode || newDrawMode != lastDrawMode)
    {
        lastSyncMode = newSyncMode;
        lastDrawMode = newDrawMode;
    }

    if (nativeStyleChanged || (getWindowHandle() != nullptr && !attributesApplied))
        setWindowAttributes();
}

void HUDWindow::setWindowAttributes()
{
#if JUCE_WINDOWS
    HWND hwnd = (HWND)getWindowHandle();
    if (hwnd != nullptr)
    {
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        style |= WS_MINIMIZEBOX | WS_SYSMENU;
        SetWindowLong(hwnd, GWL_STYLE, style);

        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_TOOLWINDOW | WS_EX_LAYERED;

        if (!currentlyInEditMode)
            exStyle |= WS_EX_TRANSPARENT;
        else
            exStyle &= ~WS_EX_TRANSPARENT;

        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
        
        // Final pixel-rendering fix 
        SetLayeredWindowAttributes(hwnd, 0, 255, 0x00000002); // LWA_ALPHA
        
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, 
                     SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOACTIVATE);
        
        attributesApplied = true;
    }
#endif
}

} // namespace CrokyScopy
