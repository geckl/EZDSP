/*
  ==============================================================================

    bufferCreator.cpp
    Created: 25 Dec 2021 7:36:10pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#include <JuceHeader.h>
#include "bufferCreator.h"
#include "../Utils/Vectors.h"
//#include "guiCreator.h"

//==============================================================================
bufferCreator::bufferCreator(guiCreator* g)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    typeValue.addItem ("int",  1);
    typeValue.addItem ("float",  2);
    
    sizeValue.setInputRestrictions(0,"0123456789");
    
    addAndMakeVisible(parametersLabel);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(sizeLabel);
    addAndMakeVisible (typeLabel);
    
    addAndMakeVisible(nameValue);
    addAndMakeVisible(sizeValue);
    addAndMakeVisible(typeValue);
    
    nameValue.setTooltip("The variable name used to access a buffer value");
    sizeValue.setTooltip("The number of values to be stored in the buffer");
    typeValue.setTooltip("The value type that the buffer stores (integer or floating point numbers)");

    createComponent.setSize(150,50);
    createComponent.setButtonText("Create");
    addAndMakeVisible(&createComponent);
    createComponent.addListener(this);

    guiWindowCallback= g;
}

bufferCreator::~bufferCreator()
{
}

void bufferCreator::paint (juce::Graphics& g)
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
    g.drawFittedText ("A buffer component is an array that can be used to store data (usually audio samples). Each buffer comes with an indexer that is initialized to zero and possesses the same name as the buffer with \"Index\" appended. This indexer is memory safe, meaning that any values outside the buffer range loop back around to the start of the buffer.", getLocalBounds().reduced(100, 50),
                juce::Justification::centred, 8);   // Explain component
}

void bufferCreator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    parametersLabel.setBounds (10, 10, getWidth() - 20, 20);
    nameLabel.setBounds(0, 40, 120, 20);
    nameValue.setBounds(120, 40, 100, 20);
    typeLabel.setBounds(0, 70, 120, 20);
    typeValue.setBounds(120, 70, 100, 20);
    sizeLabel.setBounds(0, 100, 120, 20);
    sizeValue.setBounds(120, 100, 100, 20);
    
    createComponent.setBounds(getWidth()/4, getHeight()/1.1, getWidth()/2, getHeight()/10);
}

void bufferCreator::buttonClicked(juce::Button* button)
{
    //create array of component parameters and append to array of components
    if (button == &createComponent)
    {
        if(std::find(reservedWords.begin(), reservedWords.end(), nameValue.getText()) == reservedWords.end())
        {
            juce::Array<juce::String> componentParameters;
            
            
            componentParameters.add("BUFFER");
            componentParameters.add(nameValue.getText()+"[" + nameValue.getText() + "Index]");
            componentParameters.add(typeValue.getText());
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add(sizeValue.getText());
            componentParameters.add("OFF");
            componentParameters.add("3");
            componentParameters.add(nameValue.getText());
           
            
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




