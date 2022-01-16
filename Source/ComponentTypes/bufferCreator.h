/*
  ==============================================================================

    bufferCreator.h
    Created: 25 Dec 2021 7:36:10pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../guiCreator.h"

//==============================================================================
/*
*/

class guiCreator;

class bufferCreator  : public juce::Component,
                       private juce::Button::Listener
{
public:
    bufferCreator(guiCreator* g);
    ~bufferCreator() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
    enum RadioButtonIds
    {
        typeButtons = 1001
    };
    
    juce::TextButton createComponent;
    
    juce::Label componentLabel   { {}, "Component Type: "};
    juce::Label parametersLabel  { {}, "Component Parameters: "};
    juce::Label minLabel { {}, "Min Value: "};
    juce::Label maxLabel { {}, "Max Value: "};
    juce::Label initLabel { {}, "Initial Value: "};
    juce::Label intervalLabel { {}, "Step: "};
    juce::Label nameLabel { {}, "Variable Name: "};
    juce::Label typeLabel { {}, "Data Type: " };
    juce::Label sizeLabel { {}, "Size: " };
    
    juce::ComboBox typeValue;
    
    juce::TextEditor nameValue, sizeValue;
    
    juce::TooltipWindow variableTips {this};
    
    guiCreator *guiWindowCallback;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (bufferCreator)
};
