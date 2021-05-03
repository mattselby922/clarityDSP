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
    void initialize_analyser();

    void initialize_lowPassLabel();
    void initialize_lowPass();
    void initialize_highPassLabel();
    void initialize_highPass();


    // Defining whichFilter value (used to toggle between SMA and LMS)
    // Initial filter is SMA
    bool whichFilter;


    class AnalyserComponent : public juce::AudioAppComponent,
    private juce::Timer
{
public:
    AnalyserComponent()
        : forwardFFT(fftOrder),
        window(fftSize, juce::dsp::WindowingFunction<float>::hann)
    {
        setOpaque(true);
        setAudioChannels(2, 0);  // we want a couple of input channels but no outputs
        startTimerHz(30);
     
    }

    ~AnalyserComponent() override
    {
        shutdownAudio();
    }

    //==============================================================================
    void prepareToPlay(int, double) override {}
    void releaseResources() override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        if (bufferToFill.buffer->getNumChannels() > 0)
        {
            auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

            for (auto i = 0; i < bufferToFill.numSamples; ++i)
                pushNextSampleIntoFifo(channelData[i]);
        }
    }


    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);

        g.setOpacity(1.0f);
        g.setColour(juce::Colours::white);
        drawFrame(g);
    }

    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            drawNextFrameOfSpectrum();
            nextFFTBlockReady = false;
            repaint();
        }
    }

    void pushNextSampleIntoFifo(float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next frame should now be rendered..
        if (fifoIndex == fftSize)               // [11]
        {
            if (!nextFFTBlockReady)            // [12]
            {
                juce::zeromem(fftData, sizeof(fftData));
                memcpy(fftData, fifo, sizeof(fifo));
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;             // [12]
    }

    void drawNextFrameOfSpectrum()
    {
        // first apply a windowing function to our data
        window.multiplyWithWindowingTable(fftData, fftSize);       // [1]

        // then render our FFT data..
        forwardFFT.performFrequencyOnlyForwardTransform(fftData);  // [2]

        auto mindB = -100.0f;
        auto maxdB = 0.0f;

        for (int i = 0; i < scopeSize; ++i)                         // [3]
        {
            auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
            auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
            auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
                - juce::Decibels::gainToDecibels((float)fftSize)),
                mindB, maxdB, 0.0f, 1.0f);

            scopeData[i] = level;                                   // [4]
        }
    }

    void drawFrame(juce::Graphics& g)
    {
        for (int i = 1; i < scopeSize; ++i)
        {
            auto width = getLocalBounds().getWidth();
            auto height = getLocalBounds().getHeight();

            g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
                                  juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
                          (float)juce::jmap(i,     0, scopeSize - 1, 0, width),
                                  juce::jmap(scopeData[i],     0.0f, 1.0f, (float)height, 0.0f) });
        }
    }

    enum
    {
        fftOrder = 11,             // [1]
        fftSize = 1 << fftOrder,  // [2]
        scopeSize = 200             // [3]
    };

private:
    juce::dsp::FFT forwardFFT;                      // [4]
    juce::dsp::WindowingFunction<float> window;     // [5]

    float fifo[fftSize];                           // [6]
    float fftData[2 * fftSize];                    // [7]
    int fifoIndex = 0;                              // [8]
    bool nextFFTBlockReady = false;                 // [9]
    float scopeData[scopeSize];                    // [10]

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserComponent)
};


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

    AnalyserComponent analyser;

    // For Low and High Pass Filters
    juce::Slider lowPass;
    juce::Label lowPassLabel;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassFrequency;
    juce::Slider highPass;
    juce::Label highPassLabel;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> highPassFrequency;

    // Device Manager
    DeviceManager theDeviceManager;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClarityPlugin3AudioProcessorEditor)
};