#include "ScopeComponent.h"
#include "../DSP/PluginProcessor.h"

namespace CrokyScopy
{

ScopeComponent::ScopeComponent(CrokyScopyAudioProcessor& p)
    : processor(p)
{
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);
    
    resizeConstrainer.setMinimumSize(30, 30);
    resizeConstrainer.setMaximumSize(3840, 2160);

    startTimerHz(60); // 60 FPS rendering
}

ScopeComponent::~ScopeComponent()
{
}

void ScopeComponent::paint(juce::Graphics& g)
{
    // Draw Background/Grid
    if (needsGridRepaint || gridCache.isNull()) 
        renderGridToCache();
    
    g.drawImageAt(gridCache, 0, 0);

    // Draw the Waveform
    renderWaveform(g);

    // Draw Edit Mode indicator if applicable
    bool isEditMode = processor.apvts.getRawParameterValue("edit_mode")->load() > 0.5f;
    if (isEditMode)
    {
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawRect(getLocalBounds(), 1);
        g.setFont(12.0f);
        g.drawText("EDIT MODE: Drag to move, Right-drag to resize", getLocalBounds().removeFromBottom(20).reduced(10, 0), juce::Justification::centredRight);
    }
}

void ScopeComponent::resized()
{
    needsGridRepaint = true;
}

void ScopeComponent::timerCallback()
{
    // Refresh parameters
    currentHue = processor.apvts.getRawParameterValue("hue")->load();
    currentLineWidth = processor.apvts.getRawParameterValue("line_width")->load();
    currentVerticalZoom = processor.apvts.getRawParameterValue("vertical_zoom")->load();

    repaint();
}

void ScopeComponent::renderGridToCache()
{
    gridCache = juce::Image(juce::Image::ARGB, getWidth(), getHeight(), true);
    juce::Graphics g(gridCache);

    float opacity = processor.apvts.getRawParameterValue("opacity")->load();
    g.fillAll(juce::Colours::black.withAlpha(opacity * 0.5f));

    g.setColour(juce::Colours::white.withAlpha(0.05f));
    for (int x = 0; x < getWidth(); x += 50) g.drawVerticalLine(x, 0, (float)getHeight());
    for (int y = 0; y < getHeight(); y += 50) g.drawHorizontalLine(y, 0, (float)getWidth());

    needsGridRepaint = false;
}

void ScopeComponent::renderWaveform(juce::Graphics& g)
{
    auto& buffer = processor.getScopeBuffer();
    int currentIdx = buffer.getWriteIndex();
    
    juce::Path wavePath;
    float halfH = (float)getHeight() / 2.0f;
    float w = (float)getWidth();

    // Mapping: Bin Index -> X position
    auto getX = [&](int binIdx) { return (float)binIdx / (float)CrokyScopy::ScopeBuffer::NumBins * w; };

    bool firstPoint = true;

    // We draw the path in two segments to handle the "paging" wrap-around cleanly.
    // Segment 1: from start up to current writing point
    // Segment 2: from current writing point to end
    for (int i = 0; i < CrokyScopy::ScopeBuffer::NumBins; ++i)
    {
        auto range = buffer.getBinRange(i);
        float x = getX(i);
        float yMin = halfH - (range.getEnd() * halfH * currentVerticalZoom);
        float yMax = halfH - (range.getStart() * halfH * currentVerticalZoom);

        if (firstPoint)
        {
            wavePath.startNewSubPath(x, (yMin + yMax) / 2.0f);
            firstPoint = false;
        }

        // Invisible Seam: break path at the write head to avoid horizontal connecting lines
        if (i == currentIdx)
        {
            wavePath.startNewSubPath(x, (yMin + yMax) / 2.0f);
        }
        else
        {
            wavePath.lineTo(x, yMin);
            wavePath.lineTo(x, yMax);
        }
    }

    g.setColour(juce::Colour::fromHSV(currentHue, 0.8f, 1.0f, 1.0f));
    g.strokePath(wavePath, juce::PathStrokeType(currentLineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void ScopeComponent::mouseDown(const juce::MouseEvent& e)
{
    bool isEditMode = processor.apvts.getRawParameterValue("edit_mode")->load() > 0.5f;
    if (!isEditMode) return;

    if (e.mods.isLeftButtonDown())
    {
        dragger.startDraggingComponent(getParentComponent(), e);
    }
}

void ScopeComponent::mouseDrag(const juce::MouseEvent& e)
{
    bool isEditMode = processor.apvts.getRawParameterValue("edit_mode")->load() > 0.5f;
    if (!isEditMode) return;

    if (e.mods.isLeftButtonDown())
    {
        dragger.dragComponent(getParentComponent(), e, &resizeConstrainer);
    }
    else if (e.mods.isRightButtonDown())
    {
        // Resizing
        auto* parent = getParentComponent();
        if (parent != nullptr) {
            auto pos = e.getEventRelativeTo(parent).getPosition();
            parent->setSize(juce::jmax(100, pos.getX()), juce::jmax(50, pos.getY()));
        }
    }
}

} // namespace CrokyScopy
