#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class CrokyScopyAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    CrokyScopyAudioProcessorEditor(CrokyScopyAudioProcessor&);
    ~CrokyScopyAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    CrokyScopyAudioProcessor& audioProcessor;

    // UI Components
    juce::TextButton hudToggleButton { "Show HUD Window" };
    juce::ToggleButton editModeToggle { "Enable HUD Edit Mode" };
    
    juce::Slider opacitySlider { juce::Slider::LinearHorizontal, juce::Slider::TextBoxLeft };
    juce::Label opacityLabel { "Opacity", "HUD Opacity" };

    juce::Slider lineWidthSlider { juce::Slider::LinearHorizontal, juce::Slider::TextBoxLeft };
    juce::Label lineWidthLabel { "LineWidth", "Line Width" };

    juce::ComboBox beatsCombo;
    juce::Label beatsLabel { "Beats", "Beats per Pass" };

    // APVTS Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ButtonAttachment> editModeAttachment;
    std::unique_ptr<SliderAttachment> opacityAttachment;
    std::unique_ptr<SliderAttachment> lineWidthAttachment;
    std::unique_ptr<ComboBoxAttachment> beatsAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrokyScopyAudioProcessorEditor)
};
