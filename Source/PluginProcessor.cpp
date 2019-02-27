/*
  ==============================================================================

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RompemuelasAudioProcessor::RompemuelasAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       //.withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

RompemuelasAudioProcessor::~RompemuelasAudioProcessor()
{
}

//==============================================================================
const String RompemuelasAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RompemuelasAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RompemuelasAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RompemuelasAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RompemuelasAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RompemuelasAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RompemuelasAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RompemuelasAudioProcessor::setCurrentProgram (int index)
{
}

const String RompemuelasAudioProcessor::getProgramName (int index)
{
    return {};
}

void RompemuelasAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void RompemuelasAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    suspendProcessing(true);
    
    grainBuffer   = AudioBuffer<float>::AudioBuffer();
    outputBuffer  = AudioBuffer<float>::AudioBuffer();
    
    channels         = 2;
    playBackIndex    = 0;
    repetitionsCount = 0;
}

void RompemuelasAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RompemuelasAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void RompemuelasAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    if(isSuspended() == false){
        
        buffer.clear (0, 0, buffer.getNumSamples());
        buffer.clear (1, 0, buffer.getNumSamples());
        
        auto* channelDataLeft  = buffer.getWritePointer (0);
        auto* channelDataRigth = buffer.getWritePointer (1);
        
        //outputBufferFill();
        //grainBufferFill(fileBuffer);
        for(int sample = 0; sample < buffer.getNumSamples(); ++sample){
            
            channelDataLeft[sample]  = outputBuffer.getSample(0, playBackIndex);
            channelDataRigth[sample] = outputBuffer.getSample(1, playBackIndex);
            
            playBackIndex++;
            
            if(playBackIndex >= samplesInGrain){
                updateGainValues();
                if(repetitionsCount >= repeticiones){
                    if(reverse == false){
                        grainBufferFill(fileBuffer);
                    }else{
                        grainBufferFill(reverseFileBuffer);
                    }
                    outputBufferFill();
                    repetitionsCount = 0;
                }
                playBackIndex = 0;
                ++repetitionsCount;
            }
        }
        buffer.applyGain(volume);
    }
}
void RompemuelasAudioProcessor::updateGainValues(){
    volumeMax = volumeMaxSliderValue;
    volumeMin = volumeMinSliderValue;
    if(volumeMax > volumeMin){
        volume = volumeMin + (volumeMax - volumeMin)*randomNumber.nextFloat();
    }else{
        volume = volumeMax;
    }
}
void RompemuelasAudioProcessor::updateFrameValues(){
    fileInicio = fileInicioSliderValue;
    fileFinal  = fileFinalSliderValue;
    
    if(fileInicio < fileFinal){
        grainInicio = fileInicio + randomNumber.nextInt(fileFinal - fileInicio);
    }else{
        grainInicio = fileInicio;
    }
}
void RompemuelasAudioProcessor::updateGrainValues(){
    
    samplesInGrainMin = samplesInGrainMinSliderValue + 40;
    samplesInGrainMax = samplesInGrainMaxSliderValue + 40;
    if(samplesInGrainMax > samplesInGrainMin){
        samplesInGrainSound = samplesInGrainMin + randomNumber.nextInt(samplesInGrainMax - samplesInGrainMin);
    }else{
        samplesInGrainSound = samplesInGrainMax;
    }
    
    if(samplesInGrainSound > fileFinal - fileInicio + 1){
        samplesInGrainSound = fileFinal - fileInicio;
        grainInicio         = fileInicio;
    }
    grainFinal = grainInicio + samplesInGrainSound;
    if(grainFinal > fileFinal){
        grainInicio = fileFinal - samplesInGrainSound;
    }
    
}
void RompemuelasAudioProcessor::grainBufferFill(AudioSampleBuffer inputBuffer){
    updateFrameValues();
    updateGrainValues();
    
    grainBuffer.clear();
    grainBuffer.setSize(2, samplesInGrainSound);
    
    grainBuffer.copyFrom(0, 0, inputBuffer, 0, grainInicio, samplesInGrainSound);
    grainBuffer.copyFrom(1, 0, inputBuffer, 1, grainInicio, samplesInGrainSound);
    
    grainBuffer.applyGainRamp(0, 20, 0, volume);
    grainBuffer.applyGainRamp(samplesInGrainSound - 20, 20, volume, 0);
    
    if(playBackIndex >= samplesInGrainSound){
        playBackIndex = 0;
    }
}
void RompemuelasAudioProcessor::updateDensityValues(){
    densityMin = densityMinSliderValue;
    densityMax = densityMaxSliderValue;
    
    if(densityMax > densityMin){
        density = densityMin + (densityMax - densityMin) * randomNumber.nextFloat();
    }else{
        density = densityMax;
    }
    
    samplesInGrainSilence = (1 - density) * getSampleRate();
    
    samplesInGrain = samplesInGrainSound + samplesInGrainSilence;
    if(playBackIndex >= samplesInGrain){
        playBackIndex = 0;
    }
}
void RompemuelasAudioProcessor::outputBufferFill(){
    updateDensityValues();
    
    outputBuffer.clear();
    outputBuffer.setSize(2, samplesInGrain);
    
    if(samplesInGrainSound > 0 and samplesInGrainSilence > 0){
        
        outputBuffer.copyFrom(0, 0,                   grainBuffer,       0, 0, samplesInGrainSound);
        outputBuffer.copyFrom(1, 0,                   grainBuffer,       1, 0, samplesInGrainSound);
        
        outputBuffer.copyFrom(0, samplesInGrainSound, silenceFileBuffer, 0, 0, samplesInGrainSilence);
        outputBuffer.copyFrom(1, samplesInGrainSound, silenceFileBuffer, 1, 0, samplesInGrainSilence);
        
        outputBuffer.applyGainRamp(0, 30, 0, volume);
        outputBuffer.applyGainRamp(samplesInGrainSound - 30, 30, volume, 0);
        
    }else if(samplesInGrainSound > 0 and samplesInGrainSilence == 0){
        
        outputBuffer.copyFrom(0, 0, grainBuffer,       0, 0, samplesInGrainSound);
        outputBuffer.copyFrom(1, 0, grainBuffer,       1, 0, samplesInGrainSound);
        
    }else if(samplesInGrainSound == 0 and samplesInGrainSilence > 0){
        
        outputBuffer.copyFrom(0, 0, silenceFileBuffer, 0, 0, samplesInGrainSilence);
        outputBuffer.copyFrom(1, 0, silenceFileBuffer, 1, 0, samplesInGrainSilence);
        
    }
}
//==============================================================================
bool RompemuelasAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RompemuelasAudioProcessor::createEditor()
{
    return new RompemuelasAudioProcessorEditor (*this);
}

//==============================================================================
void RompemuelasAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RompemuelasAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RompemuelasAudioProcessor();
}
