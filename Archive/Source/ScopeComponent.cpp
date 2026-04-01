#include "ScopeComponent.h"
#include "PluginProcessor.h"

namespace CrokyScopy
{

ScopeComponent::ScopeComponent(CrokyScopyAudioProcessor& p)
    : processor(p)
{
<<<<<<< Updated upstream
    setOpaque(false);
    
    // Add resizable corner ONLY for edit mode (hidden by default)
    resizer = std::make_unique<juce::ResizableCornerComponent>(this, nullptr);
    resizer->setVisible(false);
    addAndMakeVisible(*resizer);

    // Frame rate for visual updates
    startTimerHz(60);
=======
    setInterceptsMouseClicks(true, true);
    updateVisualsFromAPVTS();
    startTimerHz(60); 
>>>>>>> Stashed changes
}

ScopeComponent::~ScopeComponent()
{
}

void ScopeComponent::renderGridToCache()
{
    int w = getWidth();
    int h = getHeight();
    if (w <= 0 || h <= 0) return;

    gridCache = juce::Image(juce::Image::ARGB, w, h, true);
    juce::Graphics g(gridCache);

    g.setColour(juce::Colours::white.withAlpha(0.15f));
    
    // Draw 4 vertical lines (Time Divisions)
    for (int i = 1; i < 4; ++i)
    {
        float x = (w / 4.0f) * i;
        g.drawVerticalLine((int)x, 0.0f, (float)h);
    }

    // Draw horizontal center dash
    float dashLengths[] = { 4.0f, 4.0f };
    g.drawDashedLine(juce::Line<float>(0.0f, h * 0.5f, (float)w, h * 0.5f), dashLengths, 2, 1.0f);
}

void ScopeComponent::resized()
{
<<<<<<< Updated upstream
    // The resizer corner sits at the bottom right
    if (resizer != nullptr)
        resizer->setBounds(getWidth() - 20, getHeight() - 20, 20, 20);

    // Re-render the static background grid when size changes
=======
>>>>>>> Stashed changes
    renderGridToCache();
}

void ScopeComponent::timerCallback()
{
    repaint();
}

void ScopeComponent::updateVisualsFromAPVTS()
{
<<<<<<< Updated upstream
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
=======
    // Simplified parameter loading using working String IDs
    currentOpacity = processor.apvts.getRawParameterValue("opacity")->load();
    currentHue = processor.apvts.getRawParameterValue("hue")->load();
    currentVerticalZoom = processor.apvts.getRawParameterValue("vertical_zoom")->load();
    currentLineWidth = processor.apvts.getRawParameterValue("line_width")->load();
    currentlyInEditMode = processor.apvts.getRawParameterValue("edit_mode")->load() > 0.5f;

    // Quality-of-Life Safety Checks
    if (std::isnan(currentOpacity)) currentOpacity = 0.8f;
    if (std::isnan(currentHue)) currentHue = 0.5f;
    if (std::isnan(currentVerticalZoom)) currentVerticalZoom = 1.0f;
    if (std::isnan(currentLineWidth)) currentLineWidth = 2.0f;
    
    currentOpacity = juce::jlimit(0.1f, 1.0f, currentOpacity);
>>>>>>> Stashed changes
}

void ScopeComponent::paint(juce::Graphics& g)
{
<<<<<<< Updated upstream
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
=======
    updateVisualsFromAPVTS();

    if (!gridCache.isValid() && getWidth() > 0 && getHeight() > 0)
        renderGridToCache();

    // Background
    g.fillAll(juce::Colours::black.withAlpha(currentOpacity));

    // Grid
    if (gridCache.isValid())
    {
        g.drawImageAt(gridCache, 0, 0);
    }
    
    // Waveform
    g.setColour(juce::Colour::fromHSV(currentHue, 0.8f, 1.0f, 1.0f));
    renderWaveform(g);

    // Edit Mode Indicator
    if (currentlyInEditMode)
    {
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawRect(getLocalBounds(), 2.0f);
        g.setFont(14.0f);
        g.drawText("EDIT MODE: [DRAG] Position  [R-DRAG] Size", 10, 10, getWidth(), 20, juce::Justification::topLeft);
    }
}

void ScopeComponent::renderWaveform(juce::Graphics& g)
{
    auto& scopeBuffer = processor.scopeBuffer;
    int numBins = scopeBuffer.getNumBins();
    int currentWriteIdx = scopeBuffer.getWriteIndex();
    int drawMode = (int)processor.apvts.getRawParameterValue("draw_mode")->load();
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
    g.setColour(currentPalette.waveformColor);
    g.strokePath(wavePath, juce::PathStrokeType(currentSettings.lineWidth));
=======
    auto getX = [&](int binIndex) -> float {
        if (drawMode == 0) return (binIndex / (float)numBins) * w;
        int shiftedIdx = binIndex - currentWriteIdx;
        if (shiftedIdx < 0) shiftedIdx += numBins;
        return (shiftedIdx / (float)numBins) * w;
    };

    for (int i = 0; i < numBins; ++i)
    {
        // Avoid naming collision with 'range' keyword/symbol
        auto binRange = scopeBuffer.getBinRange(i);
        float val = (binRange.getStart() + binRange.getEnd()) * 0.5f;
        float x = getX(i);
        float y = (0.5f - (val * 0.45f * currentVerticalZoom)) * h;

        if (!pathStarted || i == (currentWriteIdx + 1) % numBins)
        {
            p.startNewSubPath(x, y);
            pathStarted = true;
        }
        else
        {
            p.lineTo(x, y);
        }
    }

    g.strokePath(p, juce::PathStrokeType(currentLineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
>>>>>>> Stashed changes
}

void ScopeComponent::mouseDown(const juce::MouseEvent& e)
{
<<<<<<< Updated upstream
    if (inEditMode && e.y < currentSettings.editModeHandleHeight)
        dragger.startDraggingComponent(this->getTopLevelComponent(), e);
=======
    if (!currentlyInEditMode) return;
    
    auto* window = getTopLevelComponent();
    if (window != nullptr)
    {
        if (e.mods.isRightButtonDown())
            resizeConstrainer.setMinimumSize(20, 10);
        else
            dragger.startDraggingComponent(window, e);
    }
>>>>>>> Stashed changes
}

void ScopeComponent::mouseDrag(const juce::MouseEvent& e)
{
<<<<<<< Updated upstream
    if (inEditMode && e.y < currentSettings.editModeHandleHeight)
        dragger.dragComponent(this->getTopLevelComponent(), e, nullptr);
=======
    if (!currentlyInEditMode) return;
    
    auto* window = getTopLevelComponent();
    if (window != nullptr)
    {
        if (e.mods.isRightButtonDown())
        {
            int newW = juce::jlimit(20, 4000, e.getScreenX() - window->getScreenX());
            int newH = juce::jlimit(10, 4000, e.getScreenY() - window->getScreenY());
            window->setSize(newW, newH);
        }
        else
        {
            dragger.dragComponent(window, e, &resizeConstrainer);
        }
    }
>>>>>>> Stashed changes
}

} // namespace CrokyScopy
