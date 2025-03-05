#pragma once
#include <JuceHeader.h>

class DrummerAIAudioProcessor : public juce::AudioProcessor {
public:
    DrummerAIAudioProcessor();
    ~DrummerAIAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    struct Pattern {
        std::vector<float> kickTimes;  // Times in seconds
        std::vector<float> snareTimes;
        std::vector<float> bassNotes;  // MIDI note numbers
        float bpm;
        float kickAmp = 0.5f;  // Default amplitudes
        float snareAmp = 0.4f;
        float bassAmp = 0.3f;
    };
    std::string lastCommand;
private:
    Pattern currentPattern;
   
    double sampleRate;

    float generateKick(float t);
    float generateSnare(float t);
    float generateBass(float t, float freq);
    float midiToFreq(float midiNote);
    Pattern generatePattern(const std::string& command, float defaultBPM = 120.0f);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrummerAIAudioProcessor)
};