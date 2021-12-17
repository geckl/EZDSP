/*
  ==============================================================================

    guiCreator.cpp
    Created: 9 Dec 2021 9:32:13pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#include <JuceHeader.h>
#include "guiCreator.h"
#include "componentCreator.h"

//==============================================================================
guiCreator::guiCreator(const juce::String& name, juce::Array<juce::Array <juce::String>> *g)
: DocumentWindow (name, juce::Colours::grey, juce::DocumentWindow::allButtons)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    addGUIComponent.setSize(150,50);
    addGUIComponent.setButtonText("Add New Component");
    addAndMakeVisible(&addGUIComponent);
    
    addGUIComponent.addListener(this);
    
    guiCodeArray=g;
}

guiCreator::~guiCreator()
{
}

void guiCreator::paint (juce::Graphics& g)
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

void guiCreator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void guiCreator::closeButtonPressed()
{
    DBG(guiCodeArray->size());
    this->getTopLevelComponent()->exitModalState(0);
    delete this;
}

void guiCreator::buttonClicked(juce::Button* button)
{
    if (button == &addGUIComponent)
    {
        //createAComponentWindow = new juce::DialogWindow("Component Creator", juce::Colours::grey, true, false);
        
        createAComponentWindow= new componentCreator(this);
        
        //createAComponentWindow->createComponent.addListener(this);
        
        juce::DialogWindow::LaunchOptions launchOptions;
        
        launchOptions.content.setOwned(createAComponentWindow);
        launchOptions.content->setSize(640, 480);
        launchOptions.launchAsync();
        
        
        DBG("Button Works 4Real");
    }
}
