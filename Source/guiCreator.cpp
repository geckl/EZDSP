/*
  ==============================================================================

    guiCreator.cpp
    Created: 9 Dec 2021 9:32:13pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#include <JuceHeader.h>
#include "guiCreator.h"

//==============================================================================
guiCreator::guiCreator(const juce::String& name, juce::Array<juce::Array <juce::String>> *g, juce::Button *b)
: DocumentWindow (name, juce::Colours::grey, juce::DocumentWindow::allButtons)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    //addGUIComponent.setSize(150,50);
    addAndMakeVisible(&addGUIComponent);
    //addGUIComponent.addListener(this);
    
    addGUIComponent.addItem ("Variable",  1);
    addGUIComponent.addItem ("Slider",   2);
    addGUIComponent.addItem ("Buffer", 3);
    addGUIComponent.addItem ("Button", 4);
    addGUIComponent.setText("Add New Component", juce::NotificationType::dontSendNotification);
    
    addGUIComponent.onChange = [this] { styleMenuChanged(); };
    //addGUIComponent.setSelectedId (1);
    
    deleteSelectedComponents.setSize(150,50);
    deleteSelectedComponents.setButtonText("Delete Selected Components");
    addAndMakeVisible(&deleteSelectedComponents);
    deleteSelectedComponents.addListener(this);
    
    guiCodeArray=g;
    runCodeButton=b;
    
    myTable= new componentTable(guiCodeArray);
    addAndMakeVisible(myTable);
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
    
}

void guiCreator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    myTable->setBounds (0,0,getWidth(),getHeight()-150);
    addGUIComponent.setBounds(50, getHeight()-100, 150, 50);
    deleteSelectedComponents.setBounds(250, getHeight()-100, 150, 50);

}

void guiCreator::closeButtonPressed()
{
    DBG(guiCodeArray->size());
    runCodeButton->triggerClick();
    this->getTopLevelComponent()->exitModalState(0);
    delete this;
}

void guiCreator::buttonClicked(juce::Button* button)
{
    /*if (button == &addGUIComponent)
    {
        //createAComponentWindow = new juce::DialogWindow("Component Creator", juce::Colours::grey, true, false);
        
        createAComponentWindow= new componentCreator(this);
        
        //createAComponentWindow->createComponent.addListener(this);
        
        juce::DialogWindow::LaunchOptions launchOptions;
        
        launchOptions.content.setOwned(createAComponentWindow);
        launchOptions.content->setSize(640, 480);
        launchOptions.launchAsync();
        
        
        DBG("Button Works 4Real");
    }*/
    if(button == &deleteSelectedComponents)
    {
        for(int i=0;i<myTable->numRows;i++)
        {
            DBG(juce::String(i) + myTable->getSelection(i));
            if(myTable->getSelection(i)=="ON")
            {
                guiCodeArray->remove(i);
                myTable->updateContent();
            }
        }
    }
}

void guiCreator::styleMenuChanged()
   {
       if(addGUIComponent.getSelectedId()>0)
       {
           switch (addGUIComponent.getSelectedId())
           {
                   
               case 1:
                   createAComponentWindow= new variableCreator(this);
                   addGUIComponent.setSelectedId(0);
                   break;
               case 2:
                   createAComponentWindow= new sliderCreator(this);
                   addGUIComponent.setSelectedId(0);
                   break;
               case 3:
                   createAComponentWindow= new bufferCreator(this);
                   addGUIComponent.setSelectedId(0);
                   break;
               case 4:
                   createAComponentWindow= new buttonCreator(this);
                   addGUIComponent.setSelectedId(0);
                   break;
               default:
                   break;
           }
           
           addGUIComponent.setText("Add New Component", juce::NotificationType::dontSendNotification);
           juce::DialogWindow::LaunchOptions launchOptions;
           launchOptions.content.setOwned(createAComponentWindow);
           launchOptions.content->setSize(640, 480);
           launchOptions.launchAsync();
       }
   }
