#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SimpleFFT.h"
#include "AnalyserComponent.h"

//==============================================================================
/**
*/
class ClarityPlugin3AudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Slider::Listener, public juce::Button::Listener
{
public:
    ClarityPlugin3AudioProcessorEditor(ClarityPlugin3AudioProcessor&);
    ~ClarityPlugin3AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // implementing the sliderValueChanged
    void sliderValueChanged(juce::Slider* slider) override;

    // implementing the buttonClicked
    void buttonClicked(juce::Button* button);

private:

    juce::Label projectName;
    
    juce::TextButton muteButton;

    // Adding the slider for the Gain
    juce::Slider mGainControlSlider;
    juce::Label gainLabel;

    SimpleFFT FFT;
    AnalyserComponent SA;

    // Filters
    juce::Slider lowPass;
    juce::Label lowPassLabel;
    juce::Slider highPass;
    juce::Label highPassLabel;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ClarityPlugin3AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClarityPlugin3AudioProcessorEditor)
};