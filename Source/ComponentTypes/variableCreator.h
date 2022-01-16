/*
  ==============================================================================

    variableCreator.h
    Created: 25 Dec 2021 7:35:52pm
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

class variableCreator  : public juce::Component,
                         private juce::Button::Listener
{
public:
    variableCreator(guiCreator* g);
    ~variableCreator() override;

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
    
    juce::TextEditor initValue, nameValue;
    
    juce::TooltipWindow variableTips {this};
    
    guiCreator *guiWindowCallback;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (variableCreator)
};
