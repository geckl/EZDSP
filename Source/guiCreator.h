/*
  ==============================================================================

    guiCreator.h
    Created: 11 Nov 2021 1:50:19pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "componentCreator.h"

//==============================================================================
/*
*/
class guiCreator  : public juce::DocumentWindow,
                    private juce::Button::Listener
{
public:
    guiCreator(const juce::String& name)
    : DocumentWindow (name, juce::Colours::grey, juce::DocumentWindow::allButtons)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        
        addGUIComponent.setSize(150,50);
        addGUIComponent.setButtonText("Add New Component");
        addAndMakeVisible(&addGUIComponent);
        
        addGUIComponent.addListener(this);
        
    }

    ~guiCreator() override
    {
        //if (componentCreator){delete componentCreator;}
    }

    void paint (juce::Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

        g.setColour (juce::Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour (juce::Colours::white);
        g.setFont (14.0f);
        g.drawText ("guiCreator", getLocalBounds(),
                    juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..

    }
    
    void closeButtonPressed() override
    {
        delete this;
    }
    
    void buttonClicked(juce::Button* button) override
    {
        if (button == &addGUIComponent)
        {
            //createAComponent = new juce::DialogWindow("Component Creator", juce::Colours::grey, true, false);
            
            createAComponent= new componentCreator();
            juce::DialogWindow::LaunchOptions launchOptions;
            
            launchOptions.content.setOwned(createAComponent);
            launchOptions.content->setSize(640, 480);
            launchOptions.launchAsync();
            
            DBG("Button Works 4Real");
        }
    }
    
    juce::TextButton addGUIComponent;
    //juce::ScopedPointer<juce::DialogWindow> createAComponentWindow;
    juce::ScopedPointer<componentCreator> createAComponent;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (guiCreator)
};
