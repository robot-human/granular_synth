/*
  ==============================================================================

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdio.h>
#include <iostream>

//==============================================================================
RompemuelasAudioProcessorEditor::RompemuelasAudioProcessorEditor (RompemuelasAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    formatManager.registerBasicFormats();
    setSize (400, 500);
    guiDef();
}

RompemuelasAudioProcessorEditor::~RompemuelasAudioProcessorEditor()
{
    openButton      .removeListener (this);
    gainSlider      .removeListener (this);
    frameSlider     .removeListener (this);
    grainSizeSlider .removeListener (this);
    densitySlider   .removeListener (this);
    repetitionSlider.removeListener (this);
}

//==============================================================================
void RompemuelasAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    g.setColour (Colours::black);
    g.fillRoundedRectangle(240, 450, 150, 40, 20);
    g.setColour (Colours::darkgrey);
    g.drawRoundedRectangle(240, 450, 150, 40, 20,5);
    g.setColour (Colours::deeppink);
    g.setFont (15.0f);
    g.drawFittedText ("Teorema sound", 240, 450,150,40, Justification::centred, 1);
}

void RompemuelasAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    openButton          .setBounds (10,                 20,  getWidth() - 20,   20);
    reverseToggle       .setBounds (10,                 50,  getWidth() - 20,   20);
    
    gainSlider          .setBounds (50,                 120, getWidth() - 100,  30);
    gainMinLabel        .setBounds (10,                 120, 100,               30);
    gainMaxLabel        .setBounds (getWidth() - 50,    120, 100,               30);
    
    frameSlider         .setBounds (50,                 190, getWidth() - 100,  30);
    frameMinLabel       .setBounds (10,                 190, 100,               30);
    frameMaxLabel       .setBounds (getWidth() - 50,    190, 100,               30);
    
    grainSizeSlider     .setBounds (50,                 260, getWidth() - 100,  30);
    grainSizeMinLabel   .setBounds (10,                 260, 100,               30);
    grainSizeMaxLabel   .setBounds (getWidth() - 50,    260, 100,               30);
    
    densitySlider       .setBounds (50,                 330, getWidth() - 100,  30);
    densityMinLabel     .setBounds (10,                 330, 100,               30);
    densityMaxLabel     .setBounds (getWidth() - 50,    330, 100,               30);
    
    repetitionSlider    .setBounds (50,                 400, getWidth() - 100,  30);
    repetitionNumLabel  .setBounds (10,                 400, getWidth() - 100,  30);
}
void RompemuelasAudioProcessorEditor::loadFile(){
    //Carga el archivo que se va a reproducir
    processor.suspendProcessing(true);
    FileChooser chooser ("Select a Wave file shorter than 2 seconds to play...",
                         File(),
                         "*.wav");
    
    if (chooser.browseForFileToOpen())
    {
        File file = chooser.getResult();
        
        std::unique_ptr<AudioFormatReader> reader (formatManager.createReaderFor (file));
        
        if (reader.get() != nullptr)
        {
            //Si se ha cargado el archivo correctamente, se inicializan valores
            isFileLoaded = true;
            processor.samplesInFile = (int) reader->lengthInSamples;
            
            //Buffers con el archivo, el archivo invertido y silencios.
            processor.fileBuffer       .setSize (processor.channels, processor.samplesInFile);
            processor.reverseFileBuffer.setSize (processor.channels, processor.samplesInFile);
            processor.silenceFileBuffer.setSize (processor.channels, processor.getSampleRate());
            
            reader->read (&processor.fileBuffer, 0, processor.samplesInFile, 0, true, true);
            
            processor.reverseFileBuffer.copyFrom (0, 0, processor.fileBuffer, 0, 0, processor.samplesInFile);
            processor.reverseFileBuffer.copyFrom (1, 0, processor.fileBuffer, 1, 0, processor.samplesInFile);
            processor.reverseFileBuffer.reverse  (0, processor.samplesInFile);
            
            processor.silenceFileBuffer.applyGain(0, processor.silenceFileBuffer.getNumSamples(), 0);
        }
        openButton.setButtonText (chooser.getResult().getFileName());
        
        //Inicializa valores del reproductor y del slider Frame
        processor.playBackIndex            = 0;
        processor.fileInicioSliderValue    = 0;
        processor.fileFinalSliderValue     = processor.samplesInFile - 1;
        processor.fileInicio               = processor.fileInicioSliderValue;
        processor.fileFinal                = processor.fileFinalSliderValue;
        
        frameSlider.setRange(processor.fileInicio, processor.fileFinal);
        frameSlider.setMinValue(processor.fileInicio);
        frameSlider.setMaxValue(processor.fileFinal);
        
        //Inicializa valores del slider Grain. Valor máximo 0.5 segundos o el tamaño del archivo.
        if(processor.samplesInFile < processor.getSampleRate()/2){
            grainSizeSlider.setRange(0, processor.samplesInFile - 40, 1);
            grainSizeSlider.setMinValue(0);
            grainSizeSlider.setMaxValue(processor.samplesInFile - 40);
            
        }else{
            grainSizeSlider.setRange(0, processor.getSampleRate()/2, 1);
            grainSizeSlider.setMinValue(0);
            grainSizeSlider.setMaxValue(processor.getSampleRate()/2);
        }
        
        
        processor.samplesInGrain = processor.samplesInFile;
        processor.outputBuffer.clear();
        processor.outputBuffer.setSize(2, processor.samplesInGrain);
        //processor.outputBuffer.copyFrom(0, 0, processor.fileBuffer, 0, 0, processor.samplesInGrain);
        //processor.outputBuffer.copyFrom(1, 0, processor.fileBuffer, 1, 0, processor.samplesInGrain);
        processor.outputBuffer.applyGain(0, 100, 0);
        
        processor.grainInicio         = processor.fileInicio;
        processor.grainFinal          = processor.fileFinal;
        processor.samplesInGrainSound = processor.getSampleRate();
        processor.grainBuffer.clear();
        processor.grainBuffer.setSize(2, processor.samplesInGrainSound);
        //processor.grainBuffer.copyFrom(0, 0, processor.fileBuffer, 0, 0, processor.samplesInGrainSound);
        //processor.grainBuffer.copyFrom(1, 0, processor.fileBuffer, 1, 0, processor.samplesInGrainSound);
        processor.grainBuffer.applyGain(0, processor.samplesInGrainSound, 0);
        
        processor.suspendProcessing(false);
    }
}
void RompemuelasAudioProcessorEditor::buttonClicked(Button *button){
    if (button == &openButton)
    {
        loadFile();
    }
}
void RompemuelasAudioProcessorEditor::sliderValueChanged(Slider *slider){
    
    if (slider == &gainSlider){
        processor.volumeMinSliderValue = gainSlider.getMinValue();
        processor.volumeMaxSliderValue = gainSlider.getMaxValue();
        gainMinLabel.setText(std::to_string(processor.volumeMinSliderValue).substr(0,4),dontSendNotification);
        gainMaxLabel.setText(std::to_string(processor.volumeMaxSliderValue).substr(0,4),dontSendNotification);
        
        
    }else if (slider == &frameSlider){
        processor.fileInicioSliderValue    = (int) frameSlider.getMinValue();
        processor.fileFinalSliderValue     = (int) frameSlider.getMaxValue();
        if(processor.fileInicioSliderValue < processor.fileFinalSliderValue){
            if(processor.isSuspended() and isFileLoaded){
                processor.suspendProcessing(false);
            }
        }else{
            processor.suspendProcessing(true);
        }
        frameMinInSecs = processor.fileInicioSliderValue/processor.getSampleRate();
        frameMaxInSecs = processor.fileFinalSliderValue/processor.getSampleRate();
        frameMinLabel.setText(std::to_string(frameMinInSecs).substr(0,5),dontSendNotification);
        frameMaxLabel.setText(std::to_string(frameMaxInSecs).substr(0,5),dontSendNotification);
        
    }else if (slider == &grainSizeSlider){
        processor.samplesInGrainMinSliderValue = grainSizeSlider.getMinValue();
        processor.samplesInGrainMaxSliderValue = grainSizeSlider.getMaxValue();
        /*
         if(processor.samplesInGrainMaxSliderValue == 0){
         processor.suspendProcessing(true);
         }else{
         if(processor.isSuspended() and isFileLoaded){
         processor.suspendProcessing(false);
         }
         }
         */
        grainSizeMinLabel.setText(std::to_string(processor.samplesInGrainMinSliderValue),dontSendNotification);
        grainSizeMaxLabel.setText(std::to_string(processor.samplesInGrainMaxSliderValue),dontSendNotification);
        
    }else if (slider == &densitySlider){
        processor.densityMinSliderValue = densitySlider.getMinValue();
        processor.densityMaxSliderValue = densitySlider.getMaxValue();
        densityMinLabel.setText(std::to_string(processor.densityMinSliderValue).substr(0,4),dontSendNotification);
        densityMaxLabel.setText(std::to_string(processor.densityMaxSliderValue).substr(0,4),dontSendNotification);
        
        
    }else if (slider == &repetitionSlider){
        processor.repeticiones = (int) repetitionSlider.getValue();
        repetitionNumLabel.setText(std::to_string(processor.repeticiones),dontSendNotification);
    }
}
void RompemuelasAudioProcessorEditor::reverseButtonChanged()
{
    processor.reverse = reverseToggle.getToggleState();
}
void RompemuelasAudioProcessorEditor::guiDef(){
    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open file...");
    openButton.addListener(this);
    
    addAndMakeVisible (&reverseToggle);
    reverseToggle.setButtonText ("Reverse file");
    reverseToggle.onClick = [this] { reverseButtonChanged(); };
    
    addAndMakeVisible (&gainSlider);
    gainSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    gainSlider.setTextBoxStyle(gainSlider.getTextBoxPosition(), true, 0, 0);
    gainSlider.setRange(0, 1, 0.001);
    gainSlider.setMinValue(0);
    gainSlider.setMaxValue(1);
    gainSlider.addListener(this);
    
    addAndMakeVisible (&gainLabel);
    gainLabel.setText("Volume",dontSendNotification);
    gainLabel.attachToComponent(&gainSlider,false);
    
    addAndMakeVisible (&gainMinLabel);
    gainMinLabel.setFont(Font(12));
    gainMinLabel.setText(std::to_string(0),dontSendNotification);
    
    addAndMakeVisible (&gainMaxLabel);
    gainMaxLabel.setFont(Font(12));
    gainMaxLabel.setText(std::to_string(1),dontSendNotification);
    
    addAndMakeVisible (&frameSlider);
    frameSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    frameSlider.setTextBoxStyle(frameSlider.getTextBoxPosition(), true, 0, 0);
    frameSlider.setRange(0, 100, 1);
    frameSlider.setMinValue(0);
    frameSlider.setMaxValue(100);
    frameSlider.addListener(this);
    
    addAndMakeVisible (&frameLabel);
    frameLabel.setText("Frame",dontSendNotification);
    frameLabel.attachToComponent(&frameSlider,false);
    
    addAndMakeVisible (&frameMinLabel);
    frameMinLabel.setFont(Font(12));
    frameMinLabel.setText(std::to_string(0),dontSendNotification);
    
    addAndMakeVisible (&frameMaxLabel);
    frameMaxLabel.setFont(Font(12));
    frameMaxLabel.setText(std::to_string(100),dontSendNotification);
    
    addAndMakeVisible (&grainSizeSlider);
    grainSizeSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    grainSizeSlider.setTextBoxStyle(grainSizeSlider.getTextBoxPosition(), true, 0, 0);
    grainSizeSlider.setRange(0, 22010, 1);
    grainSizeSlider.setMinValue(0);
    grainSizeSlider.setMaxValue(22010);
    grainSizeSlider.addListener(this);
    
    addAndMakeVisible (&grainSizeLabel);
    grainSizeLabel.setText("Grain size",dontSendNotification);
    grainSizeLabel.attachToComponent(&grainSizeSlider,false);
    
    addAndMakeVisible (&grainSizeMinLabel);
    grainSizeMinLabel.setFont(Font(12));
    grainSizeMinLabel.setText(std::to_string(0),dontSendNotification);
    
    addAndMakeVisible (&grainSizeMaxLabel);
    grainSizeMaxLabel.setFont(Font(12));
    grainSizeMaxLabel.setText(std::to_string(100),dontSendNotification);
    
    addAndMakeVisible (&densitySlider);
    densitySlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    densitySlider.setTextBoxStyle(densitySlider.getTextBoxPosition(), true, 0, 0);
    densitySlider.setRange(0, 1, 0.001);
    densitySlider.setMinValue(0);
    densitySlider.setMaxValue(1);
    densitySlider.addListener(this);
    
    addAndMakeVisible (&densityLabel);
    densityLabel.setText("Density",dontSendNotification);
    densityLabel.attachToComponent(&densitySlider,false);
    
    addAndMakeVisible (&densityMinLabel);
    densityMinLabel.setFont(Font(12));
    densityMinLabel.setText(std::to_string(0),dontSendNotification);
    
    addAndMakeVisible (&densityMaxLabel);
    densityMaxLabel.setFont(Font(12));
    densityMaxLabel.setText(std::to_string(10),dontSendNotification);
    
    
    addAndMakeVisible (&repetitionSlider);
    repetitionSlider.setRange(0,20,1);
    processor.repeticiones = 0;
    repetitionSlider.setTextBoxStyle(repetitionSlider.getTextBoxPosition(), true, 0, 0);
    repetitionSlider.addListener(this);
    
    addAndMakeVisible (&repetitionLabel);
    repetitionLabel.setText("Repetitions", dontSendNotification);
    repetitionLabel.attachToComponent(&repetitionSlider, false);
    
    addAndMakeVisible (&repetitionNumLabel);
    repetitionNumLabel.setFont(Font(12));
    repetitionNumLabel.setText(std::to_string(0), dontSendNotification);
    
    
    addAndMakeVisible (&brandLabel);
    brandLabel.setText("Algorhythmic sound", dontSendNotification);
    brandLabel.setFont(Font(15,italic));
    //brandLabel.setColour(Label::backgroundColourId, juce::Colours::darkviolet);
    brandLabel.setColour(Label::textColourId, juce::Colours::hotpink);
}

