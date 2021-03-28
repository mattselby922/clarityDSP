#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SimpleFFT.h"
#include "AnalyserComponent.h"

//==============================================================================
/**
*/
class ClarityPlugin3AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ClarityPlugin3AudioProcessorEditor(ClarityPlugin3AudioProcessor&);
    ~ClarityPlugin3AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // implementing the sliderValueChanged
    void muteButtonClicked();
    void highPass_SliderValueChanged();
    void lowPass_SliderValueChanged();
    void mGainControlSlider_SliderValueChanged();

    // Initializations
    void initialize_projectName();
    void initialize_muteButton();
    void initialize_mGainControlSlider();
    void initialize_gainLabel();
    void initialize_FFT();
    void initialize_lowPassLabel();
    void initialize_lowPass();
    void initialize_highPassLabel();
    void initialize_highPass();

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

    // Spectrogram
    SimpleFFT FFT;

    // For Low and High Pass Filters
    juce::Slider lowPass;
    juce::Label lowPassLabel;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassFrequency;
    juce::Slider highPass;
    juce::Label highPassLabel;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> highPassFrequency;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClarityPlugin3AudioProcessorEditor)
};