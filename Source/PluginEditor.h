#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h" 

class DrummerAIEditor : public juce::AudioProcessorEditor, public juce::Button::Listener {
public:
    DrummerAIEditor(DrummerAIAudioProcessor& p);  
    ~DrummerAIEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button*) override;

private:
    DrummerAIAudioProcessor& processor;  
    juce::TextEditor commandInput;
    juce::TextButton generateButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrummerAIEditor)
};