#include "PluginEditor.h"

CrokyScopeAudioProcessorEditor::CrokyScopeAudioProcessorEditor (CrokyScopeAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Configure components
    opacitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lineWidthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    hueSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    
    // Attachments
    showHudAttachment = std::make_unique<ButtonAttachment>(processor.treeState, "show_hud", showHudToggle);
    editModeAttachment = std::make_unique<ButtonAttachment>(processor.treeState, "edit_mode", editModeToggle);
    pausedAttachment = std::make_unique<ButtonAttachment>(processor.treeState, "paused", pausedToggle);
    transportSyncAttachment = std::make_unique<ButtonAttachment>(processor.treeState, "transport_sync", transportSyncToggle);

    opacityAttachment = std::make_unique<Attachment>(processor.treeState, "opacity", opacitySlider);
    lineWidthAttachment = std::make_unique<Attachment>(processor.treeState, "line_width", lineWidthSlider);
    hueAttachment = std::make_unique<Attachment>(processor.treeState, "hue", hueSlider);
    
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(processor.treeState.getParameter("beats")))
    {
        beatsComboBox.addItemList(choiceParam->choices, 1);
    }
    beatsAttachment = std::make_unique<ComboBoxAttachment>(processor.treeState, "beats", beatsComboBox);

    // Add children
    addAndMakeVisible(showHudToggle);
    addAndMakeVisible(editModeToggle);
    addAndMakeVisible(pausedToggle);
    addAndMakeVisible(transportSyncToggle);
    
    addAndMakeVisible(opacityLabel);
    addAndMakeVisible(opacitySlider);
    addAndMakeVisible(lineWidthLabel);
    addAndMakeVisible(lineWidthSlider);
    addAndMakeVisible(hueLabel);
    addAndMakeVisible(hueSlider);
    addAndMakeVisible(beatsLabel);
    addAndMakeVisible(beatsComboBox);

    // Initial size
    setSize(400, 300);

    // Listen to HUD toggle changes manually to manage the window creation
    processor.treeState.addParameterListener("show_hud", this);
    
    // Trigger initial state
    updateHudVisibility();
}

CrokyScopeAudioProcessorEditor::~CrokyScopeAudioProcessorEditor()
{
    processor.treeState.removeParameterListener("show_hud", this);
    
    // Ensure HUD is destroyed before processor is deleted and we are still on the Message Thread
    hudWindow.reset();
}

void CrokyScopeAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "show_hud") {
        juce::MessageManager::callAsync([this]() {
            updateHudVisibility();
        });
    }
}

void CrokyScopeAudioProcessorEditor::updateHudVisibility()
{
    auto* showParam = processor.treeState.getRawParameterValue("show_hud");
    bool shouldShow = showParam && showParam->load() > 0.5f;

    if (shouldShow && hudWindow == nullptr) {
        hudWindow = std::make_unique<ScopeHUDWindow>(processor);
        hudWindow->setVisible(true);
    } else if (!shouldShow && hudWindow != nullptr) {
        hudWindow.reset();
    }
}

void CrokyScopeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void CrokyScopeAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    // layout vertically
    showHudToggle.setBounds(area.removeFromTop(30));
    editModeToggle.setBounds(area.removeFromTop(30));
    pausedToggle.setBounds(area.removeFromTop(30));
    transportSyncToggle.setBounds(area.removeFromTop(30));
    
    auto row1 = area.removeFromTop(30);
    opacityLabel.setBounds(row1.removeFromLeft(80));
    opacitySlider.setBounds(row1);
    
    auto row2 = area.removeFromTop(30);
    lineWidthLabel.setBounds(row2.removeFromLeft(80));
    lineWidthSlider.setBounds(row2);
    
    auto row3 = area.removeFromTop(30);
    hueLabel.setBounds(row3.removeFromLeft(80));
    hueSlider.setBounds(row3);
    
    auto row4 = area.removeFromTop(30);
    beatsLabel.setBounds(row4.removeFromLeft(80));
    beatsComboBox.setBounds(row4);
}
