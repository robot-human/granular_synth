/*
  ==============================================================================

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class RompemuelasAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    RompemuelasAudioProcessor();
    ~RompemuelasAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    AudioSampleBuffer  fileBuffer;
    AudioSampleBuffer  reverseFileBuffer;
    AudioSampleBuffer  silenceFileBuffer;
    AudioBuffer<float> grainBuffer;
    AudioBuffer<float> outputBuffer;
    
    int  samplesInFile;
    int  channels;
    int  playBackIndex;
    //int  frameIndex;
    bool reverse = false;
    
    double  volume;
    double  volumeMin;
    double  volumeMax;
    double  volumeMinSliderValue;
    double  volumeMaxSliderValue;
    
    int     fileInicio;
    int     fileFinal;
    int     fileInicioSliderValue;
    int     fileFinalSliderValue;
    
    int     samplesInGrainMin;
    int     samplesInGrainMax;
    int     samplesInGrainMinSliderValue;
    int     samplesInGrainMaxSliderValue;
    int     samplesInGrainSound;
    
    
    double  densityMin;
    double  densityMax;
    double  densityMinSliderValue;
    double  densityMaxSliderValue;
    double  density;
    
    int     grainInicio;
    int     grainFinal;
    int     samplesInGrain;
    int     samplesInGrainSilence;
    
    int     repeticiones;
    int     repetitionsCount;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RompemuelasAudioProcessor)
    Random randomNumber;
    
    void updateGainValues();
    void updateFrameValues();
    void updateGrainValues();
    void updateDensityValues();
    void grainBufferFill(AudioSampleBuffer inputBuffer);
    void outputBufferFill();
};
