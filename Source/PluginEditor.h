/*
  ==============================================================================

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class RompemuelasAudioProcessorEditor  :    public AudioProcessorEditor,
                                            public Button::Listener,
                                            public Slider::Listener
{
public:
    RompemuelasAudioProcessorEditor (RompemuelasAudioProcessor&);
    ~RompemuelasAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked(Button *button) override;
    void sliderValueChanged(Slider *slider) override;
    
    float frameMinInSecs;
    float frameMaxInSecs;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RompemuelasAudioProcessor& processor;
    
    AudioFormatManager      formatManager;
    TextButton              openButton;
    ToggleButton            reverseToggle;
    
    Slider                  frameSlider;
    Label                   frameLabel;
    Label                   frameMinLabel;
    Label                   frameMaxLabel;
    
    Slider                  gainSlider;
    Label                   gainLabel;
    Label                   gainMinLabel;
    Label                   gainMaxLabel;
    
    Slider                  grainSizeSlider;
    Label                   grainSizeLabel;
    Label                   grainSizeMinLabel;
    Label                   grainSizeMaxLabel;
    
    Slider                  densitySlider;
    Label                   densityLabel;
    Label                   densityMinLabel;
    Label                   densityMaxLabel;
    
    Slider                  repetitionSlider;
    Label                   repetitionLabel;
    Label                   repetitionNumLabel;
    
    Label                   brandLabel;
    
    bool isFileLoaded = false;
    
    void reverseButtonChanged();
    
    void loadFile();
    void guiDef();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RompemuelasAudioProcessorEditor)
};
