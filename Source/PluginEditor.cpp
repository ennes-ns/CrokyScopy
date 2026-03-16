#include "PluginProcessor.h"
#include "PluginEditor.h"

CrokyScopeAudioProcessorEditor::CrokyScopeAudioProcessorEditor(CrokyScopeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 300);
    
    addAndMakeVisible(hudToggleButton);
    hudToggleButton.onClick = [this] { audioProcessor.toggleHUD(true); };
}

CrokyScopeAudioProcessorEditor::~CrokyScopeAudioProcessorEditor()
{
}

void CrokyScopeAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("CrokyScope Main Editor", getLocalBounds(), juce::Justification::centred, 1);
}

void CrokyScopeAudioProcessorEditor::resized()
{
    hudToggleButton.setBounds(10, 10, 100, 30);
}
