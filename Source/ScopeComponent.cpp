#include "ScopeComponent.h"
#include "PluginProcessor.h"

namespace CrokyScopy
{

ScopeComponent::ScopeComponent(CrokyScopyAudioProcessor& p)
    : processor(p)
{
    setOpaque(false);
    
    // Add resizable corner ONLY for edit mode (hidden by default)
    resizer = std::make_unique<juce::ResizableCornerComponent>(this, nullptr);
    resizer->setVisible(false);
    addAndMakeVisible(*resizer);

    // Frame rate for visual updates
    startTimerHz(60);
}

ScopeComponent::~ScopeComponent()
{
    stopTimer();
}

void ScopeComponent::resized()
{
    // The resizer corner sits at the bottom right
    if (resizer != nullptr)
        resizer->setBounds(getWidth() - 20, getHeight() - 20, 20, 20);

    // Re-render the static background grid when size changes
    renderGridToCache();
}

void ScopeComponent::timerCallback()
{
    updateVisualsFromAPVTS();
    repaint();
}

void ScopeComponent::updateVisualsFromAPVTS()
{
    // Fetch settings from APVTS (thread-safe load)
    inEditMode = processor.apvts.getRawParameterValue("edit_mode")->load() > 0.5f;
    
    float newBeats = processor.apvts.getRawParameterValue("beats")->load();
    float resolvedBeats = (newBeats < 0.5f) ? 4.0f : ((newBeats < 1.5f) ? 8.0f : 16.0f);
    
    if (resolvedBeats != currentBeats)
    {
        currentBeats = resolvedBeats;
        renderGridToCache(); // Grid depends on Beat Count
    }

    currentSettings.lineWidth = processor.apvts.getRawParameterValue("line_width")->load();
    
    // Toggle resizer visibility
    if (resizer != nullptr) resizer->setVisible(inEditMode);
}

void ScopeComponent::paint(juce::Graphics& g)
{
    // 1. Draw cached background
    if (gridCache.isValid())
        g.drawImageAt(gridCache, 0, 0);

    // 2. Draw live waveform
    renderWaveform(g);

    // 3. Draw Edit Mode UI on top
    if (inEditMode)
    {
        g.setColour(currentPalette.handleColor);
        g.fillRect(0.0f, 0.0f, (float)getWidth(), currentSettings.editModeHandleHeight);
        
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText("CrokyScopy - Edit Mode (Drag me)", 5, 0, getWidth(), (int)currentSettings.editModeHandleHeight, juce::Justification::centredLeft);
    }
}

void ScopeComponent::renderGridToCache()
{
    if (getWidth() <= 0 || getHeight() <= 0) return;

    gridCache = juce::Image(juce::Image::ARGB, getWidth(), getHeight(), true);
    juce::Graphics g(gridCache);

    g.fillAll(currentPalette.backgroundColor);
    g.setColour(currentPalette.gridColor);

    // Draw vertical beat lines
    float pixelsPerBeat = getWidth() / currentBeats;
    for (int i = 0; i <= (int)currentBeats; ++i)
    {
        float x = i * pixelsPerBeat;
        g.drawLine(x, 0.0f, x, (float)getHeight(), 1.0f);
    }

    // Draw horizontal center line
    float centerY = getHeight() * 0.5f;
    g.drawLine(0.0f, centerY, (float)getWidth(), centerY, 1.0f);
}

void ScopeComponent::renderWaveform(juce::Graphics& g)
{
    int numBins = ScopeBuffer::NumBins;
    int currentWriteIdx = processor.scopeBuffer.getWriteIndex();

    juce::Path wavePath;
    float w = (float)getWidth();
    float h = (float)getHeight();
    float halfH = h * 0.5f;

    // Build the path with 2 disconnected segments to create the "Playhead Gap"
    bool pathStarted = false;
    float gapSize = w * 0.02f; // 2% width gap

    for (int i = 0; i < numBins; ++i)
    {
        // Don't draw exactly at the write index to create a gap
        // (This makes it look like a sweeping heartbeat monitor)
        int dist = i - currentWriteIdx;
        if (dist < 0) dist += numBins;
        if (dist < (int)(numBins * 0.02f)) 
        {
            pathStarted = false; 
            continue;
        }

        auto range = processor.scopeBuffer.getBinRange(i);
        float x = (i / (float)numBins) * w;
        
        // Map Amplitude (-1.0 to 1.0) to Y axis
        float yMin = halfH - (range.getEnd() * halfH);
        float yMax = halfH - (range.getStart() * halfH);

        // Standardize Y (safety clamp)
        yMin = juce::jlimit(0.0f, h, yMin);
        yMax = juce::jlimit(0.0f, h, yMax);

        // Draw a vertical line for this bin's min/max bounds
        // Due to path logic, we just draw lines
        if (!pathStarted)
        {
            wavePath.startNewSubPath(x, yMin);
            wavePath.lineTo(x, yMax);
            pathStarted = true;
        }
        else
        {
            wavePath.lineTo(x, yMin);
            wavePath.lineTo(x, yMax);
        }
    }

    g.setColour(currentPalette.waveformColor);
    g.strokePath(wavePath, juce::PathStrokeType(currentSettings.lineWidth));
}

void ScopeComponent::mouseDown(const juce::MouseEvent& e)
{
    if (inEditMode && e.y < currentSettings.editModeHandleHeight)
        dragger.startDraggingComponent(this->getTopLevelComponent(), e);
}

void ScopeComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (inEditMode && e.y < currentSettings.editModeHandleHeight)
        dragger.dragComponent(this->getTopLevelComponent(), e, nullptr);
}

} // namespace CrokyScopy
