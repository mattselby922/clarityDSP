#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ClarityPlugin3AudioProcessor::ClarityPlugin3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), tree(*this, nullptr),
    lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(48000, 20000.0f)),
    highPassFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(48000, 20.0f))
#endif
{
    //Setting the max min and default of the gain
    addParameter(mGainParameter = new juce::AudioParameterFloat("gainID",
        "Gain",
        0.0f,
        2.0f,
        1.0));
    // Smoothing the gain, getting rid of pops
    mGainSmoothed = mGainParameter->get();


    //Setting parameters for compressor
    /*addParameter(compressorParameter = new juce::AudioParameterFloat("compressorID",
        "Compressor",
        0.0f,
        100.0f,
        0.0));

        */

    //Parameters for Hi/Lo Pass Filters
    tree.createAndAddParameter("lowPassFrequency", "LowPassFrequency", "lowPassFrequency",
        juce::NormalisableRange<float>(20.0f, 20000.0f), 20000.0f, nullptr, nullptr);
    tree.createAndAddParameter("highPassFrequency", "HighPassFrequency", "highPassFrequency",
        juce::NormalisableRange<float>(20.0f, 20000.0f), 20.0f, nullptr, nullptr);

    tree.createAndAddParameter("compressorId", "compressor", "compressor",
        juce::NormalisableRange<float>(0.0f, 100.0f), 20.0f, nullptr, nullptr);
}


ClarityPlugin3AudioProcessor::~ClarityPlugin3AudioProcessor()
{
}

//==============================================================================
const juce::String ClarityPlugin3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ClarityPlugin3AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ClarityPlugin3AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ClarityPlugin3AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ClarityPlugin3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ClarityPlugin3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ClarityPlugin3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void ClarityPlugin3AudioProcessor::setCurrentProgram(int index)
{
}

const juce::String ClarityPlugin3AudioProcessor::getProgramName(int index)
{
    return {};
}

void ClarityPlugin3AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void ClarityPlugin3AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //Russ edit - intitializing ability to use dsp module
    //dsp ProcessSpec class
    lastSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
    highPassFilter.prepare(spec);
    highPassFilter.reset();

    //compressor.prepare(spec);
    //compressor.reset();
}

void ClarityPlugin3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ClarityPlugin3AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void ClarityPlugin3AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.


    // I got rid of this outer loop, because I had to grab left and right channels at the same time (Liam)

    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{


    //left channel
    auto* channelLeft = buffer.getWritePointer(0);

    //right channel
    auto* channelRight = buffer.getWritePointer(1);

    //variables for LMS and IIR
    float error;
    float filterOutput;
    double mu = 0.25;   // CONVERGENCE RATE [This value will be changed for efficiency testing purposes]
    double pi = 2 * acos(0.0);
    int M = 5;  //Order of filter
    

    //processing
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Gain Smoothing (formula: x = x-z * (x-y) / x=smoothed value, y= target value, z= speed.
        mGainSmoothed = mGainSmoothed - 0.001 * (mGainSmoothed - mGainParameter->get());
    
        channelLeft[sample] *= mGainSmoothed;
        channelRight[sample] *= mGainSmoothed;

        DBG(*mGainParameter);

        /*
            IIR Filter
        */
        
        float desiredSignal = sin(mGainSmoothed * 0.05 * pi);   // Estimating desiredSignal as sin wave
        
        //filter the audio, storing it as filterOutput, which is then utilized in LMS algorithm to determine coefficients

        /*
            LMS
        */

        //Convolution
        //M is order of filter, W[sample] is the weight vector, and X[sample] is the data vector
        //filterOutput += (W[sample] * X[sample]);




        //Calculate Error (Difference b/w desired signal d(n) and filters output y(n)
        error = desiredSignal - filterOutput;

        //Estimate of Mean Squared Error (error**2)(n)
        for (int i = 0; i < M; i++)
        {
            //W[i] = W[i] + (mu * error * X[i]);
        }
        
 
    }
    //Low Pass Filter
    juce::dsp::AudioBlock<float> block(buffer);
    updateFilter();
    lowPassFilter.process(juce::dsp::ProcessContextReplacing <float>(block));
    highPassFilter.process(juce::dsp::ProcessContextReplacing <float>(block));
    //compressor.process(juce::dsp::ProcessContextReplacing<float>(block));

    //}   dont touch! outer loop closing bracket (Liam)

   


   
}

//==============================================================================
bool ClarityPlugin3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)

    //return false; //Changed it to false in order to disable the editor (Liam)
}

juce::AudioProcessorEditor* ClarityPlugin3AudioProcessor::createEditor()
{
    return new ClarityPlugin3AudioProcessorEditor(*this);

    //return nullptr; //Changed it to false in order to disable the editor,
    //comment above line to disable the editor (Liam) 
}

//==============================================================================
void ClarityPlugin3AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ClarityPlugin3AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ClarityPlugin3AudioProcessor();
}

void ClarityPlugin3AudioProcessor::updateFilter()
{
    float lowfreq = *tree.getRawParameterValue("lowPassFrequency");
    float highfreq = *tree.getRawParameterValue("highPassFrequency");

    *lowPassFilter.state = *juce::dsp::IIR::Coefficients <float>::makeLowPass(lastSampleRate, lowfreq);
    *highPassFilter.state = *juce::dsp::IIR::Coefficients <float>::makeHighPass(lastSampleRate, highfreq);
    //*compressor.state = *juce::dsp::Compressor<float>::Compressor(lastSampleRate, compress);
}

/*float* ClarityPlugin3AudioProcessor::IIRFilter(float*)
{
  //Stability conditions; Infinite Impulse Response filters must adhere to these
  if(mGainSmoothed >= 1)
  {
      mGainSmoothed = 0.999;
  }
  else if(mGainSmoothed <= -1)
  {
      mGainSmoothed = -0.999;
  }

  
  //call LMS function
  //Set coefficients

}
*/

/*void ClarityPlugin3AudioProcessor::LMS()
{
    float error;

    //Convolution
    //M is length of filter, 
    for(int i = 0; i < M; i++){
        Y += (W[i] * X[i]);
    }

    //Calculate Error (Difference b/w desired signal d(n) and filters output y(n)
    error = desired() - output();



}*/


