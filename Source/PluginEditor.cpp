#include "PluginEditor.h"

DrummerAIEditor::DrummerAIEditor(DrummerAIAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p) {
    addAndMakeVisible(commandInput);
    commandInput.setMultiLine(false);
    commandInput.setText("Enter command (e.g., 'funky 120 BPM')");

    addAndMakeVisible(generateButton);
    generateButton.setButtonText("Generate");
    generateButton.addListener(this);

    setSize(400, 200);
}

DrummerAIEditor::~DrummerAIEditor() {}

void DrummerAIEditor::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Drummer AI", getLocalBounds(), juce::Justification::centredTop, true);
}

void DrummerAIEditor::resized() {
    commandInput.setBounds(10, 30, 380, 30);
    generateButton.setBounds(10, 70, 100, 30);
}

void DrummerAIEditor::buttonClicked(juce::Button* button) {
    if (button == &generateButton) {
        processor.lastCommand = commandInput.getText().toStdString();
    }
}