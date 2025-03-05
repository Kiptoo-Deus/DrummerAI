#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
#include <algorithm>
#include "PluginProcessor.h"
#include "PluginEditor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DrummerAIAudioProcessor::DrummerAIAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
#endif
{
    lastCommand = "rock 120 BPM";  
    currentPattern = generatePattern(lastCommand);  
}

DrummerAIAudioProcessor::~DrummerAIAudioProcessor() {}

void DrummerAIAudioProcessor::prepareToPlay(double sr, int samplesPerBlock) {
    sampleRate = sr;
}

void DrummerAIAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DrummerAIAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}
#endif

float DrummerAIAudioProcessor::generateKick(float t) {
    return sin(2 * M_PI * 60.0f * t) * exp(-5.0f * t);
}

float DrummerAIAudioProcessor::generateSnare(float t) {
    return (rand() % 1000 / 1000.0f - 0.5f) * exp(-10.0f * t);
}

float DrummerAIAudioProcessor::generateBass(float t, float freq) {
    return sin(2 * M_PI * freq * t) * exp(-1.0f * t);
}

float DrummerAIAudioProcessor::midiToFreq(float midiNote) {
    return 440.0f * pow(2.0f, (midiNote - 69.0f) / 12.0f);
}

DrummerAIAudioProcessor::Pattern DrummerAIAudioProcessor::generatePattern(const std::string& command, float defaultBPM) {
    Pattern p;
    p.bpm = defaultBPM;

    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    std::vector<std::string> tokens;
    std::stringstream ss(cmd);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "bpm" && i > 0) {
            try {
                p.bpm = std::stof(tokens[i - 1]);
                if (p.bpm < 40.0f || p.bpm > 240.0f) p.bpm = defaultBPM;
            }
            catch (...) {
                p.bpm = defaultBPM;
            }
        }
    }

    float beatDuration = 60.0f / p.bpm;
    float density = 1.0f;

    bool hasStyle = false;
    for (const auto& word : tokens) {
        if (word == "rock") {
            p.kickTimes = { 0.0f, 2.0f * beatDuration };
            p.snareTimes = { beatDuration, 3.0f * beatDuration };
            p.bassNotes = { 36.0f, 36.0f };
            hasStyle = true;
        }
        else if (word == "funk" || word == "funky" || word == "groovy") {
            p.kickTimes = { 0.0f, beatDuration * 1.5f, 2.5f * beatDuration };
            p.snareTimes = { beatDuration, 3.0f * beatDuration };
            p.bassNotes = { 38.0f, 41.0f, 38.0f };
            hasStyle = true;
        }
        else if (word == "jazz") {
            p.kickTimes = { 0.0f, 2.5f * beatDuration };
            p.snareTimes = { beatDuration * 0.5f, 3.5f * beatDuration };
            p.bassNotes = { 43.0f, 43.0f };
            hasStyle = true;
        }
        else if (word == "hip-hop" || word == "hiphop") {
            p.kickTimes = { 0.0f, beatDuration * 2.5f };
            p.snareTimes = { beatDuration, 3.0f * beatDuration };
            p.bassNotes = { 36.0f };
            hasStyle = true;
        }
        else if (word == "fast") {
            p.bpm = std::min(p.bpm * 1.5f, 240.0f);
        }
        else if (word == "slow" || word == "chill") {
            p.bpm = std::max(p.bpm * 0.75f, 40.0f);
        }
        else if (word == "heavy" || word == "loud") {
    
        }
        else if (word == "sparse" || word == "light") {
            density = 0.5f;
        }
        else if (word == "busy" || word == "complex") {
            density = 1.5f;
        }
    }

    if (!hasStyle) {
        p.kickTimes = { 0.0f };
        p.snareTimes = { beatDuration };
        p.bassNotes = { 36.0f };
    }

    if (density < 1.0f && p.kickTimes.size() > 1) {
        p.kickTimes.resize(p.kickTimes.size() / 2);
        p.snareTimes.resize(p.snareTimes.size() / 2);
        p.bassNotes.resize(p.bassNotes.size() / 2);
    }
    else if (density > 1.0f) {
        p.kickTimes.push_back(beatDuration * 3.5f);
        p.snareTimes.push_back(beatDuration * 2.5f);
        p.bassNotes.push_back(p.bassNotes[0]);
    }

    return p;
}

void DrummerAIAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    buffer.clear();


    if (!lastCommand.empty()) {
        currentPattern = generatePattern(lastCommand);
        lastCommand.clear();
    }

    float patternDuration = (60.0f / currentPattern.bpm) * 4.0f;
    if (patternDuration <= 0.0f) patternDuration = 2.0f;  

    int numSamples = buffer.getNumSamples();
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);
    static float currentTime = 0.0f;

    for (int sample = 0; sample < numSamples; ++sample) {
        float t = currentTime + (sample / sampleRate);
        float wrappedTime = fmod(t, patternDuration);
        float output = 0.0f;

        for (float kickTime : currentPattern.kickTimes) {
            if (wrappedTime >= kickTime && wrappedTime < kickTime + 0.2f) {
                output += generateKick(wrappedTime - kickTime) * 0.5f;
            }
        }
        for (float snareTime : currentPattern.snareTimes) {
            if (wrappedTime >= snareTime && wrappedTime < snareTime + 0.1f) {
                output += generateSnare(wrappedTime - snareTime) * 0.4f;
            }
        }
        for (size_t i = 0; i < currentPattern.bassNotes.size(); ++i) {
            float bassTime = currentPattern.kickTimes[i % currentPattern.kickTimes.size()];
            if (wrappedTime >= bassTime && wrappedTime < bassTime + 0.5f) {
                output += generateBass(wrappedTime - bassTime, midiToFreq(currentPattern.bassNotes[i])) * 0.3f;
            }
        }

        left[sample] = right[sample] = output;
    }
    currentTime += numSamples / sampleRate;
}

juce::AudioProcessorEditor* DrummerAIAudioProcessor::createEditor() {
    return new DrummerAIEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new DrummerAIAudioProcessor();
}