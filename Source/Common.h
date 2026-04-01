#pragma once

#include <JuceHeader.h>

namespace CrokyScopy
{

/**
 * @brief Represents the visual configuration for the HUD and Oscilloscope.
 */
struct VisualSettings
{
    float lineWidth { 2.0f };               // Thickness of the waveform trace
    float gridAlpha { 0.3f };               // Opacity of the background grid
    float axisFontSize { 12.0f };           // Font size for labels (e.g., dB or Beat markers)
    float editModeHandleHeight { 24.0f };   // Height of the drag handle appearing in Edit Mode
    float minHUDWidth { 200.0f };           // Minimum resizable width for the HUD
    float minHUDHeight { 100.0f };          // Minimum resizable height for the HUD
};

/**
 * @brief Defines the color palette for the plugin.
 */
struct Palette
{
    // The main waveform trace color (e.g., vibrant green or blue)
    juce::Colour waveformColor { juce::Colours::cyan.withAlpha(0.9f) };

    // The background color of the HUD window itself
    juce::Colour backgroundColor { juce::Colours::transparentBlack };

    // Color of the background grid lines
    juce::Colour gridColor { juce::Colours::white.withAlpha(0.1f) };

    // Color of the text labels on the axes
    juce::Colour axisColor { juce::Colours::grey };

    // Color of the handle used in Edit Mode
    juce::Colour handleColor { juce::Colours::darkgrey.withAlpha(0.8f) };
    
    // Color of the gap/playhead marker
    juce::Colour playheadColor { juce::Colours::white.withAlpha(0.8f) };
};

} // namespace CrokyScopy

