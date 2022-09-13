/*
  ==============================================================================

    sliderCreator.cpp
    Created: 25 Dec 2021 6:22:53pm
    Author:  Garrett Eckl

  ==============================================================================
*/


#include <JuceHeader.h>
#include "sliderCreator.h"
#include "../Utils/Vectors.h"
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
    intervalValue.setInputRestrictions(0, "0123456789.");
    nameValue.setInputRestrictions(30,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
   
    sliderLabel.setFont (juce::Font (24.0f, juce::Font::bold));
    sliderLabel.setJustificationType (juce::Justification::centred);
    parametersLabel.setJustificationType (juce::Justification::centred);
    
    minLabel.attachToComponent(&minValue, true);
    maxLabel.attachToComponent(&maxValue, true);
    initLabel.attachToComponent(&initValue, true);
    intervalLabel.attachToComponent(&intervalValue, true);
    nameLabel.attachToComponent(&nameValue, true);
    
    addAndMakeVisible(sliderLabel);
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
    
    minValue.setTooltip("The slider's minimum value");
    maxValue.setTooltip("The sliders maximum value");
    initValue.setTooltip("The slider's initial value");
    intervalValue.setTooltip("The interval between slider values");
    nameValue.setTooltip("The slider's label and the variable name used to access the slider's current value");
    
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
    
    g.fillRect (0, 0, getWidth(), 125);

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawFittedText ("A slider component outputs a stream of values, meaning it's accompanying variable is constantly updated to reflect the slider's current position. Use this component to graphically control certain signal processing parameters.", getLocalBounds().reduced(50, 50),
                juce::Justification::centredTop, 8);   // Explain component
}

void sliderCreator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    sliderLabel.setBounds(0, 0, getWidth(), getHeight()*0.1);
    parametersLabel.setBounds ((getWidth()/2)-100, 150, 200, 20);
    minValue.setBounds ((getWidth()/2)-50, 180, 100, 20);
    maxValue.setBounds ((getWidth()/2)-50, 210, 100, 20);
    initValue.setBounds ((getWidth()/2)-50, 240, 100, 20);
    intervalValue.setBounds ((getWidth()/2)-50, 270, 100, 20);
    nameValue.setBounds((getWidth()/2)-50, 300, 100, 20);
    //typeLabel.setBounds(0, 330, 120, 20);
    //typeValue.setBounds(120, 330, 100, 20);
    
    createComponent.setBounds(getWidth()/4, getHeight()/1.1, getWidth()/2, getHeight()/10);
}

void sliderCreator::buttonClicked(juce::Button* button)
{
    //create array of component parameters and append to array of components
    if (button == &createComponent)
    {
        if((std::find(reservedWords.begin(), reservedWords.end(), nameValue.getText()) != reservedWords.end()) || (std::find(usedWords.begin(), usedWords.end(), nameValue.getText()) != usedWords.end()))
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "One of the values you enterred is a reserved keyword");
        } else if(nameValue.getText().isEmpty() || minValue.getText().isEmpty() || maxValue.getText().isEmpty() || initValue.getText().isEmpty())
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "Empty attribute");
        } else if(initValue.getText().containsOnly(".-") || minValue.getText().containsOnly(".-") || maxValue.getText().containsOnly(".-") || intervalValue.getText().containsOnly(".-"))
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "Not a valid floating point number");
        } else if(minValue.getText().getFloatValue() >= maxValue.getText().getFloatValue())
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "Min Value must be less than Max Value");
        } else if(initValue.getText().getFloatValue() < minValue.getText().getFloatValue() || initValue.getText().getFloatValue() > maxValue.getText().getFloatValue())
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "Initial Value must be in between Min Value and Max Value");
        } else if(intervalValue.getText().getFloatValue() > (maxValue.getText().getFloatValue() - minValue.getText().getFloatValue()))
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "Interval Value cannot be greater than the difference between Min Value and Max Value");
        } else{
            
            auto min = minValue.getText().getFloatValue();
            auto max = maxValue.getText().getFloatValue();
            auto init = initValue.getText().getFloatValue();
            auto interval = intervalValue.getText().getFloatValue();
            
            juce::Array<juce::String> componentParameters;
            
            componentParameters.add("SLIDER");
            componentParameters.add(nameValue.getText());
            //componentParameters.add(typeValue.getText());
            componentParameters.add("");
            componentParameters.add(juce::String(min));
            componentParameters.add(juce::String(max));
            componentParameters.add(juce::String(init));
            componentParameters.add(juce::String(interval));
            componentParameters.add("");
            componentParameters.add("OFF");
            componentParameters.add("1");
            componentParameters.add(nameValue.getText());
            
            guiWindowCallback->guiCodeArray->add(componentParameters);
            
            //Sort components so that variables appear last (standard vars must come after gui components in the soulpatch format)
            std::sort(guiWindowCallback->guiCodeArray->begin(), guiWindowCallback->guiCodeArray->end(),
              [](const auto& lhs, const auto& rhs) { return lhs[9] < rhs[9]; });
            
            //update table and code
            guiWindowCallback->myTable->updateContent();
            guiWindowCallback->runCodeButton->triggerClick();
            
            //close the component creator window
            delete this->findParentComponentOfClass<juce::DialogWindow>();
        }
    }
}



