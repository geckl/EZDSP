/*
  ==============================================================================

    EzdspHelp.h
    Created: 14 Jan 2022 2:34:31pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class EzdspHelp  : public juce::DocumentWindow
{
public:
    EzdspHelp(const juce::String& name) : DocumentWindow (name, juce::Colours::grey, juce::DocumentWindow::allButtons)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        
        //setSize (1000, 1000);
        
        /*helpText.setMultiLine(true);
        helpText.setReturnKeyStartsNewLine(true);
        
        helpText.setJustification(juce::Justification::centredTop);
        helpText.setFont(juce::Font(24));
        helpText.setText("EZDSP Keywords\n\n");
        
        //helpText.setFont(juce::Font(14));
        //helpText.moveCaretDown(true);
        helpText.insertTextAtCaret("BPM: lorem ipsum\n SAMPLERATE: lorem ipsum\n SAMPLESPERBEAT: lorem ipsum\n");
        addAndMakeVisible(&helpText);
        
        helpText.setReadOnly(true);*/
        
        helpSite.goToURL("https://geckl.github.io/EZDSP/");
        addAndMakeVisible(&helpSite);

    }

    ~EzdspHelp() override
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
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        
        //helpText.setBounds(0, 40, getWidth(), getHeight()-40);
        helpSite.setBounds(0, 0, getWidth(), getHeight());

    }
    
    void closeButtonPressed() override
    {
        this->getTopLevelComponent()->exitModalState(0);
        delete this;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EzdspHelp)
    
    //juce::TextEditor helpText;
    
    juce::WebBrowserComponent helpSite;
};
