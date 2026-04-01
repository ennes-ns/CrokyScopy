#include "ScopeHUDWindow.h"

ScopeHUDWindow::ScopeHUDWindow(CrokyScopeAudioProcessor& p)
    : juce::TopLevelWindow("CrokyScope HUD", true),
      processor(p)
{
    setOpaque(false);
    setDropShadowEnabled(false);
    setUsingNativeTitleBar(false);
    
    // Crucial for HUD feeling
    setAlwaysOnTop(true);
    
    // Get stored bounds
    auto* p_w = processor.treeState.getRawParameterValue("hud_w");
    auto* p_h = processor.treeState.getRawParameterValue("hud_h");
    
    float w = p_w ? p_w->load() : 400.0f;
    float h = p_h ? p_h->load() : 200.0f;
    
    setSize(static_cast<int>(w), static_cast<int>(h));
    
    localData.resize(SCOPE_BUFFER_SIZE, 0.0f);
    
    // Update at ~60 FPS
    startTimerHz(60);
}

ScopeHUDWindow::~ScopeHUDWindow()
{
    stopTimer();
    
    // Save position before destruction
    if (auto* p_x = processor.treeState.getParameter("hud_x")) p_x->setValueNotifyingHost(p_x->convertTo0to1((float)getX()));
    if (auto* p_y = processor.treeState.getParameter("hud_y")) p_y->setValueNotifyingHost(p_y->convertTo0to1((float)getY()));
    if (auto* p_w = processor.treeState.getParameter("hud_w")) p_w->setValueNotifyingHost(p_w->convertTo0to1((float)getWidth()));
    if (auto* p_h = processor.treeState.getParameter("hud_h")) p_h->setValueNotifyingHost(p_h->convertTo0to1((float)getHeight()));
}

void ScopeHUDWindow::timerCallback()
{
    auto* opacityParam = processor.treeState.getRawParameterValue("opacity");
    if (opacityParam) setAlpha(opacityParam->load());
    
    auto* editParam = processor.treeState.getRawParameterValue("edit_mode");
    bool editMode = editParam && editParam->load() > 0.5f;
    
    // Pas muis clicks door op Windows als we niet in edit mode zijn
    setInterceptsMouseClicks(editMode, editMode);
    
    for (int i = 0; i < SCOPE_BUFFER_SIZE; ++i) {
        localData[i] = processor.scopeData[i].load(std::memory_order_relaxed);
    }
    
    // Trigger paint on message thread
    repaint();
}

void ScopeHUDWindow::paint(juce::Graphics& g)
{
    float w = (float)getWidth();
    float h = (float)getHeight();
    float halfH = h / 2.0f;

    auto* editParam = processor.treeState.getRawParameterValue("edit_mode");
    bool editMode = editParam && editParam->load() > 0.5f;

    // Clear fully transparent
    g.fillAll(juce::Colours::transparentBlack);
    
    if (editMode) {
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.fillRect(getLocalBounds());
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds(), 2);
        
        g.drawText("CrokyScope HUD (Drag me) - Edit Mode ON", getLocalBounds(), juce::Justification::centredTop, true);
        
        // Resize handle lines in bottom right
        g.drawLine(w - 25, h - 5, w - 5, h - 25, 2.0f);
        g.drawLine(w - 15, h - 5, w - 5, h - 15, 2.0f);
    }
    
    auto* hueParam = processor.treeState.getRawParameterValue("hue");
    auto* lwParam = processor.treeState.getRawParameterValue("line_width");
    
    float hue = hueParam ? hueParam->load() : 0.4f;
    float lw = lwParam ? lwParam->load() : 2.0f;
    
    g.setColour(juce::Colour::fromHSV(hue, 0.8f, 1.0f, 1.0f));
    
    juce::Path p;
    int wPos = processor.writePos.load(std::memory_order_relaxed);
    
    p.startNewSubPath(0.0f, halfH);
    
    // Teken de topgolf, left to right sweep
    for (int i = 0; i < SCOPE_BUFFER_SIZE; ++i) {
        float val = localData[i];
        float x = (i / (float)(SCOPE_BUFFER_SIZE - 1)) * w;
        float y = halfH - (val * halfH);
        p.lineTo(x, y);
    }
    
    // Teken de ondergolf terug (gespiegeld/centered)
    for (int i = SCOPE_BUFFER_SIZE - 1; i >= 0; --i) {
        float val = localData[i];
        float x = (i / (float)(SCOPE_BUFFER_SIZE - 1)) * w;
        float y = halfH + (val * halfH);
        p.lineTo(x, y);
    }
    
    p.closeSubPath();
    g.fillPath(p);
    
    // Optional: Draw the playhead / sweep cursor
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    float headX = (wPos / (float)(SCOPE_BUFFER_SIZE - 1)) * w;
    g.drawLine(headX, 0.0f, headX, h, 2.0f);
}

void ScopeHUDWindow::resized()
{
}

void ScopeHUDWindow::mouseDown(const juce::MouseEvent& e)
{
    auto* editParam = processor.treeState.getRawParameterValue("edit_mode");
    bool editMode = editParam && editParam->load() > 0.5f;
    
    if (editMode) {
        // Maak het resize-gebied wat groter (40 pixels) voor gemak
        if (e.getPosition().x > getWidth() - 40 && e.getPosition().y > getHeight() - 40) {
            startBounds = getBounds();
            getProperties().set("isResizing", true);
        } else {
            getProperties().set("isResizing", false);
            dragger.startDraggingComponent(this, e);
        }
    }
}

void ScopeHUDWindow::mouseDrag(const juce::MouseEvent& e)
{
    auto* editParam = processor.treeState.getRawParameterValue("edit_mode");
    bool editMode = editParam && editParam->load() > 0.5f;
    
    if (editMode) {
        if (getProperties().contains("isResizing") && (bool)getProperties()["isResizing"]) {
            // Respecteer de startBounds van de mousedown
            int newW = juce::jmax(100, startBounds.getWidth() + e.getDistanceFromDragStartX());
            int newH = juce::jmax(50, startBounds.getHeight() + e.getDistanceFromDragStartY());
            
            setBounds(startBounds.getX(), startBounds.getY(), newW, newH);
        } else {
            dragger.dragComponent(this, e, nullptr);
        }
    }
}

void ScopeHUDWindow::mouseUp(const juce::MouseEvent& e)
{
    getProperties().set("isResizing", false);
    
    // Save current size to APVTS after mouse released
    if (auto* p_w = processor.treeState.getParameter("hud_w")) p_w->setValueNotifyingHost(p_w->convertTo0to1((float)getWidth()));
    if (auto* p_h = processor.treeState.getParameter("hud_h")) p_h->setValueNotifyingHost(p_h->convertTo0to1((float)getHeight()));
}
