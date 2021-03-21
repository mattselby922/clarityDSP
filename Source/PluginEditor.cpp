#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ClarityPlugin3AudioProcessorEditor::ClarityPlugin3AudioProcessorEditor(ClarityPlugin3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(800, 650);

    //displaying project name
    addAndMakeVisible(projectName);
    projectName.setFont(juce::Font(30.0f, juce::Font::bold));
    projectName.setText("CLARITY", juce::dontSendNotification);
    projectName.setColour(juce::Label::textColourId, juce::Colours::black);
    projectName.setJustificationType(juce::Justification::left);

    // Accessing the parameter list
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);


    //creating gain Slider
    addAndMakeVisible(mGainControlSlider);
    mGainControlSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mGainControlSlider.setRange(gainParameter->range.start, gainParameter->range.end);
    mGainControlSlider.setValue(*gainParameter);
    mGainControlSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    mGainControlSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    mGainControlSlider.addListener(this);

    //gainLabel
    addAndMakeVisible(gainLabel);
    gainLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    gainLabel.attachToComponent(&mGainControlSlider, false);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setText("Gain", juce::dontSendNotification);

    //creating mute button
    addAndMakeVisible(muteButton);
    muteButton.setButtonText("Mute");
    muteButton.addListener(this);

    //displaying FFT
    addAndMakeVisible(FFT);
    //FFT.setBounds();

    //displaying Spectrum Analyzer
    //addAndMakeVisible(SA);

 //Filters

    //lowPassLabel
    addAndMakeVisible(lowPassLabel);
    lowPassLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    lowPassLabel.attachToComponent(&lowPass, false);
    lowPassLabel.setJustificationType(juce::Justification::centred);
    lowPassLabel.setText("Low-Pass Filter", juce::dontSendNotification);
    //lowPass knob
    addAndMakeVisible(lowPass);
    lowPass.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lowPass.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    lowPass.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    lowPass.addListener(this);
    
    //highPassLabel
    addAndMakeVisible(highPassLabel);
    highPassLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    highPassLabel.attachToComponent(&highPass, false);
    highPassLabel.setJustificationType(juce::Justification::centred);
    highPassLabel.setText("High-Pass Filter", juce::dontSendNotification);
    //highPassKnob
    addAndMakeVisible(highPass);
    highPass.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    highPass.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    highPass.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    highPass.addListener(this);
}

ClarityPlugin3AudioProcessorEditor::~ClarityPlugin3AudioProcessorEditor()
{
    muteButton.removeListener(this);
    mGainControlSlider.removeListener(this);

}

//==============================================================================
void ClarityPlugin3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setGradientFill(juce::ColourGradient(juce::Colours::lightcoral, 0, 0, juce::Colours::darkcyan, 1000, 1000, true));
    g.fillAll();
    g.setFont(15.0f);
}

void ClarityPlugin3AudioProcessorEditor::resized()
{
    const int border = 20;

    juce::Rectangle<int> area = getLocalBounds();

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
    filtersArray.add(juce::FlexItem(0, 0, muteButton));               //adding mute button to flexbox
    filtersArray.add(juce::FlexItem(200, 200, mGainControlSlider));     //adding gain knob
    filtersArray.add(juce::FlexItem(50, 50, gainLabel));                //adding gainLabel
    filtersArray.add(juce::FlexItem(125, 125, lowPass));                //adding lowPass filter knob
    filtersArray.add(juce::FlexItem(125, 125, highPass));               //adding highPass filter knob

    filters.items = filtersArray;
    filters.performLayout(getLocalBounds().toFloat());

    FFT.setBounds(area.removeFromTop(100));

    //flexbox containing visualizers
    /*juce::FlexBox visualizers;
    visualizers.flexDirection = juce::FlexBox::Direction::row;
    visualizers.flexWrap = juce::FlexBox::Wrap::wrap;
    visualizers.justifyContent = juce::FlexBox::JustifyContent::center;
    visualizers.alignContent = juce::FlexBox::AlignContent::center;

    //Adding flexitems to juce Array
    juce::Array<juce::FlexItem> visualizersArray;
    visualizersArray.add(juce::FlexItem(150, 100, FFT));   //adding spectrogram to flexbox
    visualizersArray.add(juce::FlexItem(150, 100, SA));    //adding spectrum analyzer to flexbox

    visualizers.items = visualizersArray;
    visualizers.performLayout(getLocalBounds().toFloat());*/
}

// Defining sliderValueChanged

void ClarityPlugin3AudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    auto& params = processor.getParameters();

    if (slider == &mGainControlSlider)
    {
        juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
        *gainParameter = mGainControlSlider.getValue();
    }
}

void ClarityPlugin3AudioProcessorEditor::buttonClicked(juce::Button* button)
{
    auto& params = processor.getParameters();

    if (button == &muteButton)
    {
        //set gain to 0
        /*juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
        *gainParameter = 0;*/
    }
}