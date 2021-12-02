/*
  ==============================================================================

    componentCreator.h
    Created: 11 Nov 2021 6:05:19pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class componentCreator  : public juce::Component,
                          private juce::Button::Listener
{
public:
    componentCreator()
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        
        addAndMakeVisible(typeLabel);
        addAndMakeVisible(sliderComponent);
        addAndMakeVisible(buttonComponent);
        
        sliderComponent.onClick = [this] { updateToggleState (&sliderComponent, "Slider");};
        buttonComponent.onClick = [this] { updateToggleState (&buttonComponent, "Button");};
        
        sliderComponent.setRadioGroupId(typeButtons);
        buttonComponent.setRadioGroupId(typeButtons);
        
        addAndMakeVisible(rangeLabel);
        addAndMakeVisible(minLabel);
        addAndMakeVisible(maxLabel);
        addAndMakeVisible(intervalLabel);
        addAndMakeVisible(minValue);
        addAndMakeVisible(maxValue);
        addAndMakeVisible(intervalValue);
        
        
        /*rangeLabel.setText ("Ranger:", juce::dontSendNotification);
        rangeLabel.attachToComponent (&minValue, true);
        rangeLabel.setColour (juce::Label::textColourId, juce::Colours::orange);
        rangeLabel.setJustificationType (juce::Justification::right);*/
        
        minValue.setInputRestrictions(0, "0123456789.-");
        maxValue.setInputRestrictions(0, "0123456789.-");
        intervalValue.setInputRestrictions(0, "0123456789.-");
        
        createComponent.setSize(150,50);
        createComponent.setButtonText("Create");
        addAndMakeVisible(&createComponent);
        createComponent.addListener(this);

    }

    ~componentCreator() override
    {
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
        //g.drawText ("componentCreator", getLocalBounds(),
        //juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        
        typeLabel.setBounds (10, 10, getWidth() - 20, 20);
        sliderComponent.setBounds (20, 40, getWidth() - 30, 20);
        buttonComponent.setBounds (20, 70, getWidth() - 30, 20);
        
        rangeLabel.setBounds (10, 100, getWidth() - 20, 20);
        minLabel.setBounds (0, 130, 120, 20);
        minValue.setBounds (120, 130, 100, 20);
        maxLabel.setBounds (0, 170, 120, 20);
        maxValue.setBounds (120, 170, 100, 20);
        intervalLabel.setBounds (0, 200, 120, 20);
        intervalValue.setBounds (120, 200, 100, 20);
        
        createComponent.setBounds(getWidth()/4, getHeight()/1.1, getWidth()/2, getHeight()/10);

    }
    
    void updateToggleState (juce::Button* button, juce::String name)
    {
        auto state = button->getToggleState();
        juce::String stateString = state ? "ON" : "OFF";

        juce::Logger::outputDebugString (name + " Button changed to " + stateString);
    }
    
    void buttonClicked(juce::Button* button) override
    {
        if (button == &createComponent)
        {
            
            componentCode= "input event float cutoff  [[ name: \"Cut-off\", min: " + minValue.getText() +", max:" + maxValue.getText() + ", init: 0, step:" + intervalValue.getText() + "]];";
        }
    }
    
    enum RadioButtonIds
    {
        typeButtons = 1001
    };
    
    juce::TextButton createComponent;
    
    juce::Label typeLabel   { {}, "Component Type: "};
    juce::Label rangeLabel  { {}, "Component Range: "};
    juce::Label minLabel { {}, "Min Value: "};
    juce::Label maxLabel { {}, "Max Value: "};
    juce::Label intervalLabel { {}, "Step: "};
    juce::ToggleButton sliderComponent  { "Slider" },
                       buttonComponent    { "Button" };
    
    juce::TextEditor minValue, maxValue, intervalValue;
    juce::String componentCode;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (componentCreator)
};
