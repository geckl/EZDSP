/*
  ==============================================================================

    variableCreator.cpp
    Created: 25 Dec 2021 7:35:52pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#include <JuceHeader.h>
#include "variableCreator.h"
#include "../Utils/Vectors.h"
//#include "guiCreator.h"

//==============================================================================
variableCreator::variableCreator(guiCreator* g)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    typeValue.addItem ("int",  1);
    typeValue.addItem ("float",  2);
    
    
    initValue.setInputRestrictions(0, "0123456789.-");
    
   
    addAndMakeVisible(parametersLabel);
    addAndMakeVisible(initLabel);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible (typeLabel);
    
    addAndMakeVisible(initValue);
    addAndMakeVisible(nameValue);
    addAndMakeVisible(typeValue);
   

    createComponent.setSize(150,50);
    createComponent.setButtonText("Create");
    addAndMakeVisible(&createComponent);
    createComponent.addListener(this);

    guiWindowCallback= g;
}

variableCreator::~variableCreator()
{
}

void variableCreator::paint (juce::Graphics& g)
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
    g.drawFittedText ("A global variable stores a single value that can be accessed from anywhere in your code. Use it to store values that may need to change throughout duration of your signal processing.", getLocalBounds().reduced(100, 50),
                juce::Justification::centred, 8);   // Explain component
}

void variableCreator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    parametersLabel.setBounds (10, 10, getWidth() - 20, 20);
    initLabel.setBounds (0, 40, 120, 20);
    initValue.setBounds (120, 40, 100, 20);
    nameLabel.setBounds(0, 70, 120, 20);
    nameValue.setBounds(120, 70, 100, 20);
    typeLabel.setBounds(0, 100, 120, 20);
    typeValue.setBounds(120, 100, 100, 20);
    
    createComponent.setBounds(getWidth()/4, getHeight()/1.1, getWidth()/2, getHeight()/10);
}

void variableCreator::buttonClicked(juce::Button* button)
{
    //create array of component parameters and append to array of components
    if (button == &createComponent)
    {
        if(std::find(reservedWords.begin(), reservedWords.end(), nameValue.getText()) == reservedWords.end())
        {
            juce::Array<juce::String> componentParameters;
            
            
            componentParameters.add("VARIABLE");
            componentParameters.add(nameValue.getText());
            componentParameters.add(typeValue.getText());
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add(initValue.getText());
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("OFF");
            componentParameters.add("4");
           
            
            guiWindowCallback->guiCodeArray->add(componentParameters);
            
            //Sort components so that variables appear last (standard vars must come after gui components in the soulpatch format)
            std::sort(guiWindowCallback->guiCodeArray->begin(), guiWindowCallback->guiCodeArray->end(),
              [](const auto& lhs, const auto& rhs) { return lhs[9] < rhs[9]; });
            
            //update table
            guiWindowCallback->myTable->updateContent();
            
            //close the component creator window
            delete this->findParentComponentOfClass<juce::DialogWindow>();
        }
        
        else{
            //juce::AlertWindow keywordError("Error", "One of the values you enterred is a reserved keyword", juce::MessageBoxIconType::WarningIcon);
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "One of the values you enterred is a reserved keyword");
        }
    }
}




