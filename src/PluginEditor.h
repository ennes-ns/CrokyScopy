#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "ScopeHUDWindow.h"

class CrokyScopeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       public juce::AudioProcessorValueTreeState::Listener
{
public:
    CrokyScopeAudioProcessorEditor (CrokyScopeAudioProcessor&);
    ~CrokyScopeAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    // APVTS Listener
    void parameterChanged (const juce::String& parameterID, float newValue) override;

private:
    CrokyScopeAudioProcessor& processor;

    // GUI Controls
    juce::ToggleButton showHudToggle     { "Show HUD" };
    juce::ToggleButton editModeToggle    { "Edit Mode (Drag & Resize)" };
    juce::ToggleButton pausedToggle      { "Paused" };
    juce::ToggleButton transportSyncToggle{ "Transport Sync" };
    
    juce::Slider opacitySlider;
    juce::Slider lineWidthSlider;
    juce::Slider hueSlider;
    juce::ComboBox beatsComboBox;

    juce::Label opacityLabel    { {}, "Opacity" };
    juce::Label lineWidthLabel  { {}, "Line Width" };
    juce::Label hueLabel        { {}, "Hue" };
    juce::Label beatsLabel      { {}, "Beats" };

    // Attachments
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ButtonAttachment> showHudAttachment;
    std::unique_ptr<ButtonAttachment> editModeAttachment;
    std::unique_ptr<ButtonAttachment> pausedAttachment;
    std::unique_ptr<ButtonAttachment> transportSyncAttachment;
    
    std::unique_ptr<Attachment> opacityAttachment;
    std::unique_ptr<Attachment> lineWidthAttachment;
    std::unique_ptr<Attachment> hueAttachment;
    std::unique_ptr<ComboBoxAttachment> beatsAttachment;

    // The actual HUD instance managed by the Editor
    std::unique_ptr<ScopeHUDWindow> hudWindow;
    
    void updateHudVisibility();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrokyScopeAudioProcessorEditor)
};
