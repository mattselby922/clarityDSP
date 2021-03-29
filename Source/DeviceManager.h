/*
  ==============================================================================
    DeviceManager.h
    Created: 29 Mar 2021 7:40:35am
    Author:  bonzi
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class DeviceManager : public juce::AudioAppComponent,
                      public juce::ChangeListener
{
public:
    //==============================================================================
    DeviceManager()
        : audioSetupComp(deviceManager,  // This is the variable created by AudioAppComponent
            0,     // minimum input channels
            256,   // maximum input channels
            0,     // minimum output channels
            256,   // maximum output channels
            false, // ability to select midi inputs
            false, // ability to select midi output device
            false, // treat channels as stereo pairs
            false) // hide advanced options
    {
        //addAndMakeVisible(audioSetupComp);
        addChildComponent(audioSetupComp);
        addAndMakeVisible(diagnosticsBox);

        diagnosticsBox.setMultiLine(true);
        diagnosticsBox.setReturnKeyStartsNewLine(true);
        diagnosticsBox.setReadOnly(true);
        diagnosticsBox.setScrollbarsShown(true);
        diagnosticsBox.setCaretVisible(false);
        diagnosticsBox.setPopupMenuEnabled(true);
        diagnosticsBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
        diagnosticsBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
        diagnosticsBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

        //setSize(760, 360);
        //setSize(getLocalBounds().getWidth(), getLocalBounds().getHeight());
        setAudioChannels(2, 2);
        deviceManager.addChangeListener(this);
    }

    ~DeviceManager() override
    {
        deviceManager.removeChangeListener(this);
        shutdownAudio();
    }

    void prepareToPlay(int, double) override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        auto* device = deviceManager.getCurrentAudioDevice();

        auto activeInputChannels = device->getActiveInputChannels();
        auto activeOutputChannels = device->getActiveOutputChannels();

        auto maxInputChannels = activeInputChannels.countNumberOfSetBits();
        auto maxOutputChannels = activeOutputChannels.countNumberOfSetBits();

        for (auto channel = 0; channel < maxOutputChannels; ++channel)
        {
                bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
    }

    void releaseResources() override {}

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::grey);
        g.fillRect(getLocalBounds());
    }

    void resized() override
    {
        diagnosticsBox.setBounds(getLocalBounds());
    }

private:
    void changeListenerCallback(juce::ChangeBroadcaster*) override
    {
        dumpDeviceInfo();
    }

    static juce::String getListOfActiveBits(const juce::BigInteger& b)
    {
        juce::StringArray bits;

        for (auto i = 0; i <= b.getHighestBit(); ++i)
            if (b[i])
                bits.add(juce::String(i));

        return bits.joinIntoString(", ");
    }


    void dumpDeviceInfo()
    {
        if (auto* device = deviceManager.getCurrentAudioDevice())
        {
            logMessage("Audio device: " + device->getName().quoted());
            logMessage("Sample rate: " + juce::String(device->getCurrentSampleRate()) + " Hz");
            logMessage("Block size: " + juce::String(device->getCurrentBufferSizeSamples()) + " samples");
            logMessage("Bit depth: " + juce::String(device->getCurrentBitDepth()));
            logMessage("Input channel names: " + device->getInputChannelNames().joinIntoString(", "));
            logMessage("Active input channels: " + getListOfActiveBits(device->getActiveInputChannels()));
            logMessage("Output channel names: " + device->getOutputChannelNames().joinIntoString(", "));
            logMessage("Active output channels: " + getListOfActiveBits(device->getActiveOutputChannels()));
        }
        else
        {
            logMessage("No audio device open");
        }
    }

    void logMessage(const juce::String& m)
    {
        diagnosticsBox.moveCaretToEnd();
        diagnosticsBox.insertTextAtCaret(m + juce::newLine);
    }

    //==========================================================================
    juce::AudioDeviceSelectorComponent audioSetupComp;
    juce::TextEditor diagnosticsBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceManager)
};