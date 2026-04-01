#include "HUDWindow.h"
#include "../DSP/PluginProcessor.h"

namespace CrokyScopy
{

HUDWindow::HUDWindow(CrokyScopyAudioProcessor& p)
    : DocumentWindow("CrokyScopy HUD", juce::Colours::transparentBlack, 0), // No title bar buttons
      processor(p)
{
    setUsingNativeTitleBar(false);
    setTitleBarHeight(0); // Hide the JUCE title bar entirely for a clean scope look
    setAlwaysOnTop(true);
    setResizable(true, true);

    scopeComponent = std::make_unique<ScopeComponent>(processor);
    scopeComponent->setSize(800, 300);
    setContentNonOwned(scopeComponent.get(), true);
    
    // JUCE-native transparency
    setBackgroundColour(juce::Colours::transparentBlack);
    
    // Explicitly center the window on the main display
    centreWithSize(800, 300);
}

HUDWindow::~HUDWindow()
{
    clearContentComponent();
}

void HUDWindow::closeButtonPressed()
{
    // The user clicked the implicit close, signal the processor to shut it down safely
    processor.apvts.getRawParameterValue("show_hud")->store(0.0f);
}

void HUDWindow::resized()
{
    if (scopeComponent != nullptr)
        scopeComponent->setBounds(getLocalBounds());
}

void HUDWindow::moved()
{
    // Minimal logic -> No heavy saving unless required later.
}

void HUDWindow::updateAppearance(float opacity)
{
    // Native JUCE alpha blending - fast, cross-platform
    setAlpha(opacity);
}

} // namespace CrokyScopy
