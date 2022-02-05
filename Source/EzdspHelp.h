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
class EzdspHelp : public juce::DocumentWindow
{
public:
    EzdspHelp(const juce::String& name, int hostWidth, int hostHeight)
    : DocumentWindow (name, juce::Colours::grey, juce::DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        centreWithSize(hostWidth, hostHeight);
        setAlwaysOnTop(true);
        
        helpSite.goToURL("https://geckl.github.io/EZDSP/Documentation");
        helpSite.setSize(getWidth(), getHeight());
        setContentOwned(&helpSite, false);
        
        setVisible(true);
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
        
        //helpSite.setBounds(getLocalBounds());

    }
    
    void closeButtonPressed() override
    {
        this->getTopLevelComponent()->exitModalState(0);
        delete this;
    }
    
    void focusOfChildComponentChanged (FocusChangeType cause) override
    {
        
        if(juce::Process::isForegroundProcess())
        {
            setAlwaysOnTop(true);
        }
        else
        {
            setAlwaysOnTop(false);
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EzdspHelp)
    
    //juce::TextEditor helpText;
    
    juce::WebBrowserComponent helpSite;
};



