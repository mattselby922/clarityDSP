#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ClarityPlugin3AudioProcessorEditor::ClarityPlugin3AudioProcessorEditor(ClarityPlugin3AudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    lowPassFrequency(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "lowPassFrequency", lowPass)),
    highPassFrequency(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "highPassFrequency", highPass))
{
    setSize(800, 650);

    initialize_projectName();
    initialize_muteButton();
    initialize_mGainControlSlider();
    initialize_gainLabel();
    initialize_FFT();


    // Filters
    initialize_lowPassLabel();
    initialize_lowPass(); //low pass knob
    initialize_highPassLabel();
    initialize_highPass();
}

ClarityPlugin3AudioProcessorEditor::~ClarityPlugin3AudioProcessorEditor()
{

}

//==============================================================================
void ClarityPlugin3AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setGradientFill(juce::ColourGradient(juce::Colours::lightcoral, 0, 0, juce::Colours::darkcyan, 1000, 1000, true));
    g.fillAll();
    g.setFont(15.0f);

}

void ClarityPlugin3AudioProcessorEditor::resized()
{
    juce::Rectangle<int> area = getLocalBounds();

    // Add Padding
    auto applicationBorderPadding = 15;
    area.removeFromTop(applicationBorderPadding);
    area.removeFromLeft(applicationBorderPadding);
    area.removeFromRight(applicationBorderPadding);
    area.removeFromBottom(applicationBorderPadding);

    // projectName Label
    projectName.setBounds(area.removeFromTop(area.getHeight() / 20));

    // SimpleFFT (i.e., Spectrogram)
    FFT.setBounds(area.removeFromTop(area.getHeight() / 3));

    area.removeFromTop(100); // Temporary Spacing

    // muteButton
    //muteButton.setBounds(area.removeFromLeft(area.getWidth() / 5));
    auto buttonArea = area.removeFromLeft(area.getWidth() / 4);
    muteButton.setBounds(buttonArea.removeFromBottom(buttonArea.getHeight() / 2));

    // mGainControlSlider
    mGainControlSlider.setBounds(area.removeFromLeft(area.getWidth() / 3));

    // lowPass
    lowPass.setBounds(area.removeFromLeft(area.getWidth() / 2));

    // highPass
    highPass.setBounds(area);

    /*
    //Positioning projectName
    projectName.setBounds(area.removeFromTop(area.getHeight() / 20));

    //flexbox for EQ
    juce::FlexBox EQ;
    EQ.flexDirection = juce::FlexBox::Direction::row;
    EQ.flexWrap = juce::FlexBox::Wrap::wrap;
    EQ.alignContent = juce::FlexBox::AlignContent::flexEnd;

    juce::Array<juce::FlexItem> EQArray;
    //EQArray.add(juce::FlexItem(200,200, ));   //adding
    EQ.items = EQArray;
    EQ.performLayout(getLocalBounds().toFloat());

    //flexbox for filters
    juce::FlexBox filters;
    filters.flexDirection = juce::FlexBox::Direction::row;
    filters.flexWrap = juce::FlexBox::Wrap::wrap;
    filters.alignContent = juce::FlexBox::AlignContent::flexEnd;

    juce::Array<juce::FlexItem> filtersArray;
    filtersArray.add(juce::FlexItem(100, 0, muteButton));               //adding mute button to flexbox
    filtersArray.add(juce::FlexItem(200, 200, mGainControlSlider));     //adding gain knob
    filtersArray.add(juce::FlexItem(50, 50, gainLabel));                //adding gainLabel
    filtersArray.add(juce::FlexItem(125, 125, lowPass));                //adding lowPass filter knob
    filtersArray.add(juce::FlexItem(125, 125, highPass));               //adding highPass filter knob

    filters.items = filtersArray;
    filters.performLayout(getLocalBounds().toFloat());

    FFT.setBounds(area.removeFromTop(100));
    */
}



void ClarityPlugin3AudioProcessorEditor::muteButtonClicked()
{
    auto& params = processor.getParameters();

    juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);

    //if user mutes, make sure that unmute function does not get called
    int count = 0;

    //function to mute
    if (*gainParameter != 0) {
        *gainParameter = 0;
        muteButton.setButtonText("Unmute");
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::indianred);
        addAndMakeVisible(muteButton);


        //allows program to avoid unmute function being called
        count++;
    }

    //function to unmute
    if (*gainParameter == 0 && count == 0)
    {
        *gainParameter = mGainControlSlider.getValue();
        muteButton.setButtonText("Mute");
        addAndMakeVisible(muteButton);
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(101, 201, 134));
    }
}

void ClarityPlugin3AudioProcessorEditor::highPass_SliderValueChanged()
{

}

void ClarityPlugin3AudioProcessorEditor::lowPass_SliderValueChanged()
{
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* lpParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    *lpParameter = lowPass.getValue();
}

void ClarityPlugin3AudioProcessorEditor::mGainControlSlider_SliderValueChanged()
{
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    *gainParameter = mGainControlSlider.getValue();

    //unmutes if the user moves the rotary slider
    muteButton.setButtonText("Mute");
    addAndMakeVisible(muteButton);
    muteButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(101, 201, 134));

}

void ClarityPlugin3AudioProcessorEditor::initialize_projectName()
{
    addAndMakeVisible(projectName);
    projectName.setFont(juce::Font(30.0f, juce::Font::bold));
    projectName.setText("CLARITY", juce::dontSendNotification);
    projectName.setColour(juce::Label::textColourId, juce::Colours::black);
    projectName.setJustificationType(juce::Justification::centred);
}

void ClarityPlugin3AudioProcessorEditor::initialize_muteButton()
{
    addAndMakeVisible(muteButton);
    muteButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(101, 201, 134));
    muteButton.setButtonText("Mute");
    muteButton.onClick = [this] { muteButtonClicked(); };
}

void ClarityPlugin3AudioProcessorEditor::initialize_mGainControlSlider()
{
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    addAndMakeVisible(mGainControlSlider);
    mGainControlSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    mGainControlSlider.setRange(gainParameter->range.start, gainParameter->range.end);
    mGainControlSlider.setValue(*gainParameter);
    mGainControlSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    mGainControlSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    mGainControlSlider.onValueChange = [this] { mGainControlSlider_SliderValueChanged(); };
    getLookAndFeel().setColour(juce::Slider::backgroundColourId, juce::Colours::black);
}

void ClarityPlugin3AudioProcessorEditor::initialize_gainLabel()
{
    addAndMakeVisible(gainLabel);
    getLookAndFeel().setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    gainLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    gainLabel.attachToComponent(&mGainControlSlider, false);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setText("Gain", juce::dontSendNotification);
}

void ClarityPlugin3AudioProcessorEditor::initialize_FFT()
{
    addAndMakeVisible(FFT);
}

void ClarityPlugin3AudioProcessorEditor::initialize_lowPassLabel()
{
    addAndMakeVisible(lowPassLabel);
    lowPassLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    lowPassLabel.attachToComponent(&lowPass, false);
    lowPassLabel.setJustificationType(juce::Justification::centred);
    lowPassLabel.setText("Low-Pass Filter", juce::dontSendNotification);
}

void ClarityPlugin3AudioProcessorEditor::initialize_lowPass()
{
    //lowPass knob
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::black);
    addAndMakeVisible(&lowPass);
    lowPass.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lowPass.setRange(20.0f, 20000.0f);
    lowPass.setValue(20000.0f);
    lowPass.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    lowPass.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    lowPass.onValueChange = [this] { lowPass_SliderValueChanged(); };
}

void ClarityPlugin3AudioProcessorEditor::initialize_highPassLabel()
{
    addAndMakeVisible(highPassLabel);
    highPassLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    highPassLabel.attachToComponent(&highPass, false);
    highPassLabel.setJustificationType(juce::Justification::centred);
    highPassLabel.setText("High-Pass Filter", juce::dontSendNotification);
}

void ClarityPlugin3AudioProcessorEditor::initialize_highPass()
{
    //highPassKnob
    addAndMakeVisible(&highPass);
    highPass.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    highPass.setRange(20.0f, 20000.0f);
    highPass.setValue(20.0f);
    highPass.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    highPass.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    highPass.onValueChange = [this] { highPass_SliderValueChanged(); };
}
