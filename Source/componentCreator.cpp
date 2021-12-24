/*
  ==============================================================================

    componentCreator.cpp
    Created: 9 Dec 2021 9:31:59pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#include <JuceHeader.h>
#include "componentCreator.h"
#include "guiCreator.h"

//==============================================================================
componentCreator::componentCreator(guiCreator* g)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    addAndMakeVisible(componentLabel);
    addAndMakeVisible(variableComponent);
    addAndMakeVisible(sliderComponent);
    addAndMakeVisible(bufferComponent);
    addAndMakeVisible(buttonComponent);
    
    
    
    
    variableComponent.onClick = [this] { updateToggleState (&variableComponent, "Variable");};
    sliderComponent.onClick = [this] { updateToggleState (&sliderComponent, "Slider");};
    buttonComponent.onClick = [this] { updateToggleState (&buttonComponent, "Button");};
    bufferComponent.onClick = [this] { updateToggleState (&bufferComponent, "Buffer");};
    
    variableComponent.setRadioGroupId(typeButtons);
    sliderComponent.setRadioGroupId(typeButtons);
    bufferComponent.setRadioGroupId(typeButtons);
    buttonComponent.setRadioGroupId(typeButtons);
    
    addAndMakeVisible(parametersLabel);
    addAndMakeVisible(minLabel);
    addAndMakeVisible(maxLabel);
    addAndMakeVisible(initLabel);
    addAndMakeVisible(intervalLabel);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible (typeLabel);
    
    addAndMakeVisible(minValue);
    addAndMakeVisible(maxValue);
    addAndMakeVisible(initValue);
    addAndMakeVisible(intervalValue);
    addAndMakeVisible(nameValue);
    
    typeValue.addItem ("int",  1);
    typeValue.addItem ("float",  2);
    addAndMakeVisible(typeValue);
    
    addAndMakeVisible(sizeLabel);
    addAndMakeVisible(sizeValue);
    
    
    minValue.setInputRestrictions(0, "0123456789.-");
    maxValue.setInputRestrictions(0, "0123456789.-");
    initValue.setInputRestrictions(0, "0123456789.-");
    intervalValue.setInputRestrictions(0, "0123456789.-");
    sizeValue.setInputRestrictions(0,"0123456789");
    
    
    createComponent.setSize(150,50);
    createComponent.setButtonText("Create");
    addAndMakeVisible(&createComponent);
    createComponent.addListener(this);

    guiWindowCallback= g;
}

componentCreator::~componentCreator()
{
}

void componentCreator::paint (juce::Graphics& g)
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
    g.drawText ("componentCreator", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void componentCreator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    componentLabel.setBounds (10, 10, getWidth() - 20, 20);
    variableComponent.setBounds (20, 40, getWidth() - 30, 20);
    sliderComponent.setBounds (20, 70, getWidth() - 30, 20);
    bufferComponent.setBounds(20,100, getWidth() - 30, 20);
    buttonComponent.setBounds (20, 130, getWidth() - 30, 20);
    
    
    parametersLabel.setBounds (10, 160, getWidth() - 20, 20);
    minLabel.setBounds (0, 190, 120, 20);
    minValue.setBounds (120, 190, 100, 20);
    maxLabel.setBounds (0, 220, 120, 20);
    maxValue.setBounds (120, 220, 100, 20);
    initLabel.setBounds (0, 250, 120, 20);
    initValue.setBounds (120, 250, 100, 20);
    intervalLabel.setBounds (0, 280, 120, 20);
    intervalValue.setBounds (120, 280, 100, 20);
    nameLabel.setBounds(0, 310, 120, 20);
    nameValue.setBounds(120, 310, 100, 20);
    typeLabel.setBounds(0, 340, 120, 20);
    typeValue.setBounds(120, 340, 100, 20);
    sizeLabel.setBounds(0, 370, 120, 20);
    sizeValue.setBounds(120, 370, 100, 20);
    
    createComponent.setBounds(getWidth()/4, getHeight()/1.1, getWidth()/2, getHeight()/10);
}

void componentCreator::updateToggleState (juce::Button* button, juce::String name)
{
    auto state = button->getToggleState();
    juce::String stateString = state ? "ON" : "OFF";
    
    //Only activate input fields that are needed for given type (Standard variable, slider, or button)
    if(stateString=="ON")
    {
        if(button->getName()=="Global Variable")
        {
            minValue.setEnabled(false);
            maxValue.setEnabled(false);
            intervalValue.setEnabled(false);
            initValue.setEnabled(true);
            typeValue.setEnabled(true);
            sizeValue.setEnabled(false);
            
        }
        else if(button->getName()=="Slider")
        {
            minValue.setEnabled(true);
            maxValue.setEnabled(true);
            intervalValue.setEnabled(true);
            initValue.setEnabled(true);
            typeValue.setEnabled(false);
            sizeValue.setEnabled(false);
            
        }
        else if(button->getName()=="Buffer")
        {
            minValue.setEnabled(false);
            maxValue.setEnabled(false);
            intervalValue.setEnabled(false);
            initValue.setEnabled(false);
            typeValue.setEnabled(true);
            sizeValue.setEnabled(true);
        }
        else if(button->getName()=="Button")
        {
            minValue.setEnabled(false);
            maxValue.setEnabled(false);
            intervalValue.setEnabled(false);
            initValue.setEnabled(false);
            typeValue.setEnabled(false);
            sizeValue.setEnabled(false);
            
        }
    }

    juce::Logger::outputDebugString (name + " Button changed to " + stateString);
}

void componentCreator::buttonClicked(juce::Button* button)
{
    //create array of component parameters and append to array of components
    if (button == &createComponent)
    {
        
        juce::Array<juce::String> componentParameters;
        
        if (variableComponent.getToggleState()==true)
        {
            componentParameters.add("2VARIABLE");
            componentParameters.add(nameValue.getText());
            componentParameters.add(typeValue.getText());
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add(initValue.getText());
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("OFF");
        }
        else if (sliderComponent.getToggleState()==true)
        {
            componentParameters.add("1SLIDER");
            componentParameters.add(nameValue.getText());
            componentParameters.add("");
            componentParameters.add(minValue.getText());
            componentParameters.add(maxValue.getText());
            componentParameters.add(initValue.getText());
            componentParameters.add(intervalValue.getText());
            componentParameters.add("");
            componentParameters.add("OFF");
        }
        else if (bufferComponent.getToggleState()==true)
        {
            componentParameters.add("2BUFFER");
            componentParameters.add(nameValue.getText());
            componentParameters.add(typeValue.getText());
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add(sizeValue.getText());
            componentParameters.add("OFF");
        }
        else if (buttonComponent.getToggleState()==true)
        {
            componentParameters.add("1BUTTON");
            componentParameters.add(nameValue.getText());
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("");
            componentParameters.add("OFF");
        }
        
        
        guiWindowCallback->guiCodeArray->add(componentParameters);
        
        //Sort components so that variables appear last (standard vars must come after gui components in the soulpatch format)
        std::sort(guiWindowCallback->guiCodeArray->begin(), guiWindowCallback->guiCodeArray->end(),
          [](const auto& lhs, const auto& rhs) { return lhs[0] < rhs[0]; });
        
        //update table
        guiWindowCallback->myTable->updateContent();
        
        //close the component creator window
        delete this->findParentComponentOfClass<juce::DialogWindow>();
    }
}



