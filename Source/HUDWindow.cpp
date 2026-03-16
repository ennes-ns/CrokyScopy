#include "HUDWindow.h"

HUDWindow::HUDWindow(const juce::String& name, juce::Colour backgroundColour, int buttonsNeeded)
    : DocumentWindow(name, backgroundColour, buttonsNeeded)
{
    setUsingNativeTitleBar(false);
    setAlwaysOnTop(true);
    setResizable(true, true);
    
    // Add a simple component for the VU meter later
    // setContentOwned(new VUMeterComponent(), true);

#if JUCE_WINDOWS
    setWindowAttributes();
#endif
}

HUDWindow::~HUDWindow()
{
}

void HUDWindow::closeButtonPressed()
{
    // Handle close if needed
}

#if JUCE_WINDOWS
#include <Windows.h>

void HUDWindow::setWindowAttributes()
{
    HWND hwnd = (HWND)getWindowHandle();
    if (hwnd != nullptr)
    {
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_LAYERED | WS_EX_TOOLWINDOW;
        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
    }
}
#endif
