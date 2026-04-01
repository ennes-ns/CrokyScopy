#include "ScopeComponent.h"
#include "../DSP/PluginProcessor.h"

namespace CrokyScopy
{

ScopeComponent::ScopeComponent(CrokyScopyAudioProcessor& p)
    : processor(p)
{
    setOpaque(false);
    
    // Smooth 60FPS refresh via safe JUCE timer
    startTimerHz(60); 
}

ScopeComponent::~ScopeComponent()
{
    stopTimer();
}

void ScopeComponent::timerCallback()
{
    repaint();
}

void ScopeComponent::paint(juce::Graphics& g)
{
    // Minimal draw logic
    auto bounds = getLocalBounds().toFloat();
    
    // Background (if needed, but usually handled by HUDWindow)
    g.fillAll(juce::Colours::transparentBlack);

    const auto& buffer = processor.getScopeBuffer();
    
    float hue = processor.apvts.getRawParameterValue("hue")->load();
    float lineWidth = processor.apvts.getRawParameterValue("line_width")->load();
    
    g.setColour(juce::Colour::fromHSV(hue, 1.0f, 1.0f, 1.0f));

    wavePath.clear();
    
    int numBins = ScopeBuffer::NumBins;
    float widthRatio = bounds.getWidth() / (float)numBins;
    float centerY = bounds.getHeight() / 2.0f;
    
    // Find the latest write index to ensure scrolling
    int currentWriteIdx = buffer.getWriteIndex();

    bool started = false;
    for (int i = 0; i < numBins; ++i)
    {
        // Read oldest to newest
        int readIdx = (currentWriteIdx + i) % numBins;
        float val = buffer.getBinValue(readIdx);

        float x = i * widthRatio;
        // Simple unipolar plotting for now to ensure visibility
        float y = centerY - (val * centerY);

        if (!started)
        {
            wavePath.startNewSubPath(x, y);
            started = true;
        }
        else
        {
            wavePath.lineTo(x, y);
        }
    }

    g.strokePath(wavePath, juce::PathStrokeType(lineWidth));
}

void ScopeComponent::resized()
{
    // No allocation here yet, simple resize.
}

void ScopeComponent::mouseDown(const juce::MouseEvent& e)
{
    dragger.startDraggingComponent(getTopLevelComponent(), e);
}

void ScopeComponent::mouseDrag(const juce::MouseEvent& e)
{
    dragger.dragComponent(getTopLevelComponent(), e, nullptr);
}

} // namespace CrokyScopy
