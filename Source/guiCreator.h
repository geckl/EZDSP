/*
  ==============================================================================

    guiCreator.h
    Created: 9 Dec 2021 9:32:13pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "componentCreator.h"

//==============================================================================
/*
*/

class componentCreator;

class guiCreator  : public juce::DocumentWindow,
                     private juce::Button::Listener
{
public:
    guiCreator(const juce::String& name);
    ~guiCreator() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void closeButtonPressed() override;
    void buttonClicked(juce::Button* button) override;
    
    juce::TextButton addGUIComponent;
    //juce::ScopedPointer<juce::DialogWindow> createAComponentWindow;
    juce::ScopedPointer<componentCreator> createAComponent;
    juce::String guiCodeLine;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (guiCreator)
};
