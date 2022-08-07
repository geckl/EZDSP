/*
  ==============================================================================

    buttonCreator.cpp
    Created: 25 Dec 2021 7:36:25pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#include <JuceHeader.h>
#include "buttonCreator.h"
#include "../Utils/Vectors.h"
#include "Types.h"
//#include "guiCreator.h"

//==============================================================================
buttonCreator::buttonCreator(guiCreator* g)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    

   
    addAndMakeVisible(parametersLabel);
    addAndMakeVisible(nameLabel);
    
    nameValue.setInputRestrictions(30,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
    
    addAndMakeVisible(nameValue);
    
    nameValue.setTooltip("The button's label and the variable name used to access the button's current value ");
   

    createComponent.setSize(150,50);
    createComponent.setButtonText("Create");
    addAndMakeVisible(&createComponent);
    createComponent.addListener(this);

    guiWindowCallback= g;
}

buttonCreator::~buttonCreator()
{
}

void buttonCreator::paint (juce::Graphics& g)
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
    g.drawFittedText ("A button component is a simple binary switch (On=1, Off=0). Buttons are great for controlling the flow of your signal processing, use them to turn certain features on and off.", getLocalBounds().reduced(100, 50),
                juce::Justification::centred, 8);   // Explain component
}

void buttonCreator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    parametersLabel.setBounds (10, 10, getWidth() - 20, 20);
    nameLabel.setBounds(0, 40, 120, 20);
    nameValue.setBounds(120, 40, 100, 20);
    
    createComponent.setBounds(getWidth()/4, getHeight()/1.1, getWidth()/2, getHeight()/10);
}

void buttonCreator::buttonClicked(juce::Button* button)
{
    //create array of component parameters and append to array of components
    if (button == &createComponent)
    {
        if((std::find(reservedWords.begin(), reservedWords.end(), nameValue.getText()) == reservedWords.end()) && (std::find(usedWords.begin(), usedWords.end(), nameValue.getText()) == usedWords.end()))
        {
            
            const Button buttonComponent;
            buttonComponent.name = nameValue.getText();
            
            /*juce::Array<juce::String> componentParameters;
            
            componentParameters.add("BUTTON");
            componentParameters.add(nameValue.getText());
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("OFF");
            componentParameters.add("2");
            componentParameters.add(nameValue.getText());*/
           
            
            guiWindowCallback->guiCodeArray->add(buttonComponent);
            
            //Sort components so that variables appear last (standard vars must come after gui components in the soulpatch format)
            std::sort(guiWindowCallback->guiCodeArray->begin(), guiWindowCallback->guiCodeArray->end(),
              [](const auto& lhs, const auto& rhs) { return lhs[9] < rhs[9]; });
            
            //update table and code
            guiWindowCallback->myTable->updateContent();
            guiWindowCallback->runCodeButton->triggerClick();
            
            //close the component creator window
            delete this->findParentComponentOfClass<juce::DialogWindow>();
        }
        
        else{
            //juce::AlertWindow keywordError("Error", "One of the values you enterred is a reserved keyword", juce::MessageBoxIconType::WarningIcon);
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "One of the values you enterred is a reserved keyword");
        }
    }
}




