
#pragma once

#include <JuceHeader.h>
#include "dRowAudio/dRowAudio.h"

//==============================================================================
/**
*/

class ClarityPlugin3AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ClarityPlugin3AudioProcessor();
    ~ClarityPlugin3AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateFilter();

    // SMA Algorithm
    float* createCache(int period); // Make sure allocate and free cache for SMA
    float simpleMovingAverage1(int period, float dataPoint, float* cache);
    float simpleMovingAverage2(int period, float dataPoint, float* cache);


    //initialize IIRFilter function
    //float* IIRFilter(float*);

    //LMS function
    //void LMS();

    juce::AudioProcessorValueTreeState tree;


    // SMA
    drow::AudioOscilloscope grapher1;
    drow::AudioOscilloscope grapher2;
    float* cache1;
    float* cache2;


    //void getGrapher(drow::AudioOscilloscope grapher1, drow::AudioOscilloscope grapher2, float* cache1, float* cache2);

private:
    // Defining AudioProcessorFloat of Gain
    juce::AudioParameterFloat* mGainParameter;
    float mGainSmoothed;

    // Defining AudioProcessorFloat of compressor
    juce::AudioParameterFloat* compressorParameter;

    // Defining AudioProcessorFloat for Filters
    juce::AudioParameterFloat* lowPassFrequencyParameter;
    juce::AudioParameterFloat* highPassFrequencyParameter;


    // intitializing dsp::ProcessSpec for filters
    double lastSampleRate;


    // duplicated to analyze in stereo
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> lowPassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> highPassFilter;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClarityPlugin3AudioProcessor)
};
