#include "PluginProcessor.h"
#include "PluginEditor.h"

CrokyScopyAudioProcessorEditor::CrokyScopyAudioProcessorEditor(CrokyScopyAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
<<<<<<< Updated upstream
    setSize(400, 300);
=======
    setSize(450, 750);
>>>>>>> Stashed changes

    // 1. HUD Toggle
    addAndMakeVisible(hudToggleButton);
    hudToggleButton.onClick = [this] { audioProcessor.toggleHUD(true); };

    // 2. Edit Mode Toggle
    addAndMakeVisible(editModeToggle);
    editModeAttachment = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "edit_mode", editModeToggle);

    // 3. Opacity Slider
    addAndMakeVisible(opacityLabel);
    opacityLabel.attachToComponent(&opacitySlider, true);
    addAndMakeVisible(opacitySlider);
    opacityAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "opacity", opacitySlider);

    // 4. Line Width Slider
    addAndMakeVisible(lineWidthLabel);
    lineWidthLabel.attachToComponent(&lineWidthSlider, true);
    addAndMakeVisible(lineWidthSlider);
    lineWidthAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "line_width", lineWidthSlider);

    // 5. Beats Combo Box
    beatsCombo.addItem("4 Beats", 1);
    beatsCombo.addItem("8 Beats", 2);
    beatsCombo.addItem("16 Beats", 3);
    addAndMakeVisible(beatsLabel);
    beatsLabel.attachToComponent(&beatsCombo, true);
    addAndMakeVisible(beatsCombo);
    beatsAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "beats", beatsCombo);
}

CrokyScopyAudioProcessorEditor::~CrokyScopyAudioProcessorEditor()
{
}

void CrokyScopyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("CrokyScopy Settings", 10, 10, getWidth() - 20, 30, juce::Justification::centredLeft);
}

void CrokyScopyAudioProcessorEditor::resized()
{
    int y = 50;
    int indent = 100;
    int rowHeight = 30;
    int width = getWidth() - indent - 20;

    hudToggleButton.setBounds(10, y, width + indent, rowHeight);
    y += rowHeight + 10;

<<<<<<< Updated upstream
    editModeToggle.setBounds(10, y, width + indent, rowHeight);
    y += rowHeight + 10;
=======
    area.removeFromTop(30); // Visuals gap
    
    // VISUALS
    auto visArea = area.removeFromTop(250);
    
    opacityLabel.setBounds(visArea.removeFromTop(20));
    opacitySlider.setBounds(visArea.removeFromTop(30));
    
    hueLabel.setBounds(visArea.removeFromTop(20));
    hueSlider.setBounds(visArea.removeFromTop(30));
    
    verticalZoomLabel.setBounds(visArea.removeFromTop(20));
    verticalZoomSlider.setBounds(visArea.removeFromTop(30));
    
    lineWidthLabel.setBounds(visArea.removeFromTop(20));
    lineWidthSlider.setBounds(visArea.removeFromTop(30));
>>>>>>> Stashed changes

    opacitySlider.setBounds(indent, y, width, rowHeight);
    y += rowHeight + 10;

    lineWidthSlider.setBounds(indent, y, width, rowHeight);
    y += rowHeight + 10;

    beatsCombo.setBounds(indent, y, width, rowHeight);
}
