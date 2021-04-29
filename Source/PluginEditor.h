#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SimpleFFT.h"
#include "DeviceManager.h"
#include "AnalyserComponent.h"

//==============================================================================
/**
*/

class ClarityPlugin3AudioProcessorEditor : public juce::AudioProcessorEditor, public ClarityPlugin3AudioProcessor
{
public:
    ClarityPlugin3AudioProcessorEditor(ClarityPlugin3AudioProcessor&);
    ~ClarityPlugin3AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // implementing the sliderValueChanged
    void muteButtonClicked();
    void filterToggleClicked();
    void highPass_SliderValueChanged();
    void lowPass_SliderValueChanged();
    void mGainControlSlider_SliderValueChanged();
    void compressorSlider_SliderValueChanged();


    // Initializations
    void initialize_projectName();
    void initialize_muteButton();
    void initialize_mGainControlSlider();
    void initialize_gainLabel();

    void initialize_compressorSlider();
    void initialize_compressorLabel();

    void initialize_filterToggle();

    void initialize_FFT();

    // Grapher stuff
    void initialize_grapher();
    void getGrapher();

    void initialize_lowPassLabel();
    void initialize_lowPass();
    void initialize_highPassLabel();
    void initialize_highPass();

    // Defining whichFilter value (used to toggle between SMA and LMS)
    // Initial filter is SMA
    bool whichFilter;

private:

    // "Order of declaration matters here for variable initialization in constructor" - William

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ClarityPlugin3AudioProcessor& audioProcessor;

    // General
    juce::Label projectName;

    juce::TextButton muteButton;

    // Gain
    juce::Slider mGainControlSlider;
    juce::Label gainLabel;

    // Compressor
    juce::Slider compressorSlider;
    juce::Label compressorLabel;

    // Filter toggle
    juce::TextButton filterToggle;

    // Spectrogram
    SimpleFFT FFT;

    // For Low and High Pass Filters
    juce::Slider lowPass;
    juce::Label lowPassLabel;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassFrequency;
    juce::Slider highPass;
    juce::Label highPassLabel;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> highPassFrequency;

    // Device Manager
    DeviceManager theDeviceManager;

    // Oscilloscopes
    /*drow::AudioOscilloscope grapher1;
    drow::AudioOscilloscope grapher2;*/
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClarityPlugin3AudioProcessorEditor)
};