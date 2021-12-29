/*
  ==============================================================================

    sliderCreator.cpp
    Created: 25 Dec 2021 6:22:53pm
    Author:  Garrett Eckl

  ==============================================================================
*/


#include <JuceHeader.h>
#include "sliderCreator.h"
//#include "guiCreator.h"

//==============================================================================
sliderCreator::sliderCreator(guiCreator* g)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    //typeValue.addItem ("int",  1);
    //typeValue.addItem ("float",  2);
    
    minValue.setInputRestrictions(0, "0123456789.-");
    maxValue.setInputRestrictions(0, "0123456789.-");
    initValue.setInputRestrictions(0, "0123456789.-");
    intervalValue.setInputRestrictions(0, "0123456789.-");
    
   
    addAndMakeVisible(parametersLabel);
    addAndMakeVisible(minLabel);
    addAndMakeVisible(maxLabel);
    addAndMakeVisible(initLabel);
    addAndMakeVisible(intervalLabel);
    addAndMakeVisible(nameLabel);
    //addAndMakeVisible(typeLabel);
    
    addAndMakeVisible(minValue);
    addAndMakeVisible(maxValue);
    addAndMakeVisible(initValue);
    addAndMakeVisible(intervalValue);
    addAndMakeVisible(nameValue);
    //addAndMakeVisible(typeValue);
   

    createComponent.setSize(150,50);
    createComponent.setButtonText("Create");
    addAndMakeVisible(&createComponent);
    createComponent.addListener(this);

    guiWindowCallback= g;
}

sliderCreator::~sliderCreator()
{
}

void sliderCreator::paint (juce::Graphics& g)
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
    g.drawText ("sliderCreator", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void sliderCreator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    parametersLabel.setBounds (10, 10, getWidth() - 20, 20);
    minLabel.setBounds (0, 40, 120, 20);
    minValue.setBounds (120, 40, 100, 20);
    maxLabel.setBounds (0, 70, 120, 20);
    maxValue.setBounds (120, 70, 100, 20);
    initLabel.setBounds (0, 100, 120, 20);
    initValue.setBounds (120, 100, 100, 20);
    intervalLabel.setBounds (0, 130, 120, 20);
    intervalValue.setBounds (120, 130, 100, 20);
    nameLabel.setBounds(0, 160, 120, 20);
    nameValue.setBounds(120, 160, 100, 20);
    //typeLabel.setBounds(0, 180, 120, 20);
    //typeValue.setBounds(120, 180, 100, 20);
    
    createComponent.setBounds(getWidth()/4, getHeight()/1.1, getWidth()/2, getHeight()/10);
}

void sliderCreator::buttonClicked(juce::Button* button)
{
    //create array of component parameters and append to array of components
    if (button == &createComponent)
    {
        
        juce::Array<juce::String> componentParameters;
        
        
        componentParameters.add("SLIDER");
        componentParameters.add(nameValue.getText());
        //componentParameters.add(typeValue.getText());
        componentParameters.add("");
        componentParameters.add(minValue.getText());
        componentParameters.add(maxValue.getText());
        componentParameters.add(initValue.getText());
        componentParameters.add(intervalValue.getText());
        componentParameters.add("");
        componentParameters.add("OFF");
        componentParameters.add("1");
       
        
        guiWindowCallback->guiCodeArray->add(componentParameters);
        
        //Sort components so that variables appear last (standard vars must come after gui components in the soulpatch format)
        std::sort(guiWindowCallback->guiCodeArray->begin(), guiWindowCallback->guiCodeArray->end(),
          [](const auto& lhs, const auto& rhs) { return lhs[9] < rhs[9]; });
        
        //update table
        guiWindowCallback->myTable->updateContent();
        
        //close the component creator window
        delete this->findParentComponentOfClass<juce::DialogWindow>();
    }
}



