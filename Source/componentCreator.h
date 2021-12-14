/*
  ==============================================================================

    componentCreator.h
    Created: 9 Dec 2021 9:31:59pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "guiCreator.h"

//==============================================================================
/*
*/

class guiCreator;

class componentCreator  : public juce::Component,
                           private juce::Button::Listener
{
public:
    componentCreator(guiCreator* g);
    ~componentCreator() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void updateToggleState (juce::Button* button, juce::String name);
    void buttonClicked(juce::Button* button) override;
    
    enum RadioButtonIds
    {
        typeButtons2 = 1001
    };
    
    juce::TextButton createComponent;
    
    juce::Label typeLabel   { {}, "Component Type: "};
    juce::Label rangeLabel  { {}, "Component Range: "};
    juce::Label minLabel { {}, "Min Value: "};
    juce::Label maxLabel { {}, "Max Value: "};
    juce::Label intervalLabel { {}, "Step: "};
    juce::Label nameLabel { {}, "Variable Name: "};
    juce::ToggleButton sliderComponent  { "Slider" },
                       buttonComponent    { "Button" };
    
    juce::TextEditor minValue, maxValue, intervalValue, nameValue;
    
    guiCreator *guiWindowCallback;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (componentCreator)
};
