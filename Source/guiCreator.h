/*
  ==============================================================================

    guiCreator.h
    Created: 9 Dec 2021 9:32:13pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ComponentTypes/variableCreator.h"
#include "ComponentTypes/sliderCreator.h"
#include "ComponentTypes/bufferCreator.h"
#include "ComponentTypes/buttonCreator.h"
#include "componentTable.h"

//==============================================================================
/*
*/

class variableCreator;
class sliderCreator;
class bufferCreator;
class buttonCreator;

class guiCreator  : public juce::DocumentWindow,
                    private juce::Button::Listener
{
public:
    guiCreator(const juce::String& name, juce::Array<juce::Array <juce::String>> *g, juce::Button *b);
    ~guiCreator() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void closeButtonPressed() override;
    void buttonClicked(juce::Button* button) override;
    
    void styleMenuChanged();
    void focusOfChildComponentChanged (FocusChangeType cause) override;
    
    juce::TextButton deleteSelectedComponents, closeWindow;
    juce::ComboBox addGUIComponent;
    
    juce::Component::SafePointer<juce::Component> createAComponentWindow;
    juce::Array<juce::Array <juce::String>> *guiCodeArray;
    juce::Button *runCodeButton;
    
    juce::Component::SafePointer<componentTable> myTable;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (guiCreator)
    
};
