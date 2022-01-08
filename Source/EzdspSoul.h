/*
  ==============================================================================

    EzdspSoul.h
    Created: 11 Feb 2021 10:50:00pm
    Author:  Garrett Eckl

  ==============================================================================
 
 
 #include "../../soul_patch.h"
 #include "soul_patch_AudioProcessor.h"
 #include "soul_patch_Utilities.h"
 #include "soul_patch_CompilerCacheFolder.h"

*/

#pragma once


#ifndef JUCE_AUDIO_PROCESSORS_H_INCLUDED
 #error "this header is designed to be included in JUCE projects that contain the juce_audio_processors module"
#endif


#include "../SOUL-master/include/soul/soul_patch.h"
#include "../SOUL-master/include/soul/patch/helper_classes/soul_patch_AudioProcessor.h"
#include "../SOUL-master/include/soul/patch/helper_classes/soul_patch_Utilities.h"
#include "../SOUL-master/include/soul/patch/helper_classes/soul_patch_CompilerCacheFolder.h"

#include "guiCreator.h"
#include "Utils/EzdspCodeTokenizer.h"


namespace soul
{
namespace patch
{

//==============================================================================
/**
    This is a juce::AudioProcessor which can told to dynamically load and run different
    patches. The purpose is that you can build a native (VST/AU/etc) plugin with this
    class which can then load (and hot-reload) any SOUL patch at runtime.

    On startup, the plugin will also check in its folder for any sibling .soulpatch files,
    and if it finds exactly one, it'll load it automatically.

    The PatchLibrary template is a mechanism for providing different JIT engines.
    The PatchLibraryDLL is an example of a class that could be used for this parameter.
*/

template <typename PatchLibrary>
class EZDSPPlugin  : public juce::AudioProcessor
{
public:
    EZDSPPlugin (PatchLibrary&& library)
        : patchLibrary (std::move (library))
    {
        enableAllBuses();
        updatePatchName();
        checkForSiblingPatch();

        
        presetCode.copyFileTo(tempCode.getFile());
        presetPatch.copyFileTo(tempPatch.getFile());
        
        
        //set input and output streams to SOUL file
        input= std::unique_ptr<juce::FileInputStream> (tempCode.getFile().createInputStream());
        output= std::unique_ptr<juce::FileOutputStream> (tempCode.getFile().createOutputStream());
        
        //set new .soulpatch file to link up with new .soul file
        juce::var parsedJson= juce::JSON::parse(tempPatch.getFile());
        auto obj(parsedJson["soulPatchV1"].getDynamicObject());
        //auto answer=obj->getProperties().size();
        obj->setProperty("source",tempCode.getFile().getRelativePathFrom(tempPatch.getFile()));
        tempPatch.getFile().replaceWithText(juce::JSON::toString(parsedJson));
        
        loadCode();
        
        //Create an initial gain slider component
        juce::Array<juce::String> initialComponentParameters;
        initialComponentParameters.add("SLIDER");
        initialComponentParameters.add("GAIN");
        initialComponentParameters.add("float");
        initialComponentParameters.add("-60.0");
        initialComponentParameters.add("10.0");
        initialComponentParameters.add("0.0");
        initialComponentParameters.add("0.1");
        initialComponentParameters.add("");
        initialComponentParameters.add("OFF");
        guiArray.add(initialComponentParameters);

        //Load soulpatch properties into ValueTree
        state = juce::ValueTree (ids.SOULPatchPlugin);
        state.setProperty (ids.patchURL, tempPatch.getFile().getFullPathName(), nullptr);
        state.setProperty(ids.patchCode, fullCode.getAllContent(), nullptr);
        
        //store array of components in vars and add to ValueTree
        juce::Array<juce::var> componentsStorage;
        for(int i=0;i<guiArray.size();i++)
        {
            juce::Array<juce::var> parametersStorage;
            for(int j=0;j<guiArray[i].size();j++)
            {
                //DBG("String Array Size: ");
                //DBG(guiArray[i].size());
                parametersStorage.add(guiArray[i][j]);
            }
            componentsStorage.add(parametersStorage);
        }
        juce::var tempStorage= componentsStorage;
        state.setProperty(ids.patchComponents, tempStorage, nullptr);
        
        updatePatchState();
        
    }

    ~EZDSPPlugin() override
    {
        //tempCode.get
        plugin.reset();
        patchInstance = nullptr;
    }

    //==============================================================================
    /** Sets a new .soulpatch file or URL for the plugin to load. */
    void setPatchURL (const std::string& newFileOrURL)
    {
        //DBG(newFileOrURL);
        if (newFileOrURL != state.getProperty (ids.patchURL).toString().toStdString())
        {
            state = juce::ValueTree (ids.SOULPatchPlugin);
            state.setProperty (ids.patchURL, newFileOrURL.c_str(), nullptr);
            updatePatchState();
        }
    }
    
    //loads code from input stream into three CodeDocuments, one containing the full soul code, one containing only the DSP, and one containing only the GUI components
    void loadCode ()
    {
        int flag1 = 0;
        int flag2 = 0;
        juce::TextEditor tempFullCode, tempDspCode, tempGuiCode;
        tempFullCode.setMultiLine(true);
        tempDspCode.setMultiLine(true);
        juce::int64 tempDspEndPosition=0, tempDspStartPosition=0, tempGuiEndPosition=0, tempGuiStartPosition=0;
        
        //Read .soul file line by line. Use Markers to add select lines to DSP and GUI CodeDocuments.
        
        input->setPosition(0);
        while (! input->isExhausted())
            {
                auto line = input->readNextLine();
                if (line.startsWith ("//ENDDSP"))
                {
                    flag1=0;
                    tempDspEndPosition=input->getPosition()-line.length()-3;
                }
                
                if(flag1==1)
                {
                    // append the text to the textContent
                    tempDspCode.insertTextAtCaret (line + juce::newLine);
                }
                
                if (line.startsWith ("//BEGINDSP"))
                {
                    flag1=1;
                    tempDspStartPosition=input->getPosition();
                }
                
                if (line.startsWith ("//ENDGUI"))
                {
                    flag2=0;
                    tempGuiEndPosition=input->getPosition()-line.length()-1;
                }
                
                if(flag2==1)
                {
                    // append the text to the textContent
                    tempGuiCode.insertTextAtCaret (line + juce::newLine);
                }
                
                if (line.startsWith ("//BEGINGUI"))
                {
                    flag2=1;
                    tempGuiStartPosition=input->getPosition();
                }
                
                tempFullCode.insertTextAtCaret(line + juce::newLine);
            }
        
        fullCode.replaceAllContent(tempFullCode.getText());
        dspCode.replaceAllContent(tempDspCode.getText());
        guiCode.replaceAllContent(tempGuiCode.getText());
        
        //Record the Start/End positions of DSP and GUI sections. These are used to replace the sections when user changes the plugin
        dspEndPosition= juce::CodeDocument::Position(fullCode,tempDspEndPosition);
        dspStartPosition= juce::CodeDocument::Position(fullCode,tempDspStartPosition);
        
        guiEndPosition= juce::CodeDocument::Position(fullCode,tempGuiEndPosition);
        guiStartPosition= juce::CodeDocument::Position(fullCode,tempGuiStartPosition);
        
        //This tracks the position of these markers. As code is added their position within the document will change
        dspEndPosition.setPositionMaintained(true);
        dspStartPosition.setPositionMaintained(true);
        
        guiEndPosition.setPositionMaintained(true);
        guiStartPosition.setPositionMaintained(true);
        
    }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::Array<juce::String> initialComponentParameters;
        
        initialComponentParameters.add("VARIABLE");
        initialComponentParameters.add("SAMPLERATE");
        initialComponentParameters.add("int");
        initialComponentParameters.add("");
        initialComponentParameters.add("");
        initialComponentParameters.add(juce::String(getSampleRate()));
        initialComponentParameters.add("");
        initialComponentParameters.add("");
        initialComponentParameters.add("OFF");
        initialComponentParameters.add("4");
        
        int flag=0;
        for(int i=0;i<guiArray.size();i++)
        {
            if(guiArray.getReference(i)[1]=="SAMPLERATE")
            {
                guiArray.set(i,initialComponentParameters);
                flag++;
                break;
            }
        }
        
        if(flag==0)
        {
            guiArray.add(initialComponentParameters);
        }
        
        recalculateBpmFlag=1;
        
        if (plugin != nullptr)
            plugin->prepareToPlay (sampleRate, samplesPerBlock);
    }

    void releaseResources() override
    {
        if (plugin != nullptr)
            plugin->releaseResources();
    }

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        return plugin == nullptr || plugin->isBusesLayoutSupported (layouts);
    }

    void processBlock (juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override
    {
        /*if(recalculateBpmFlag==1)
        {
            juce::AudioPlayHead* playHead;
            juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;
            double bpm;
            
            playHead = this->getPlayHead();
            playHead->getCurrentPosition (currentPositionInfo);
            bpm = currentPositionInfo.bpm;
            
            juce::Array<juce::String> initialComponentParameters;
            
            initialComponentParameters.add("VARIABLE");
            initialComponentParameters.add("SAMPLES_PER_BEAT");
            initialComponentParameters.add("float");
            initialComponentParameters.add("");
            initialComponentParameters.add("");
            initialComponentParameters.add(juce::String((60/bpm)*getSampleRate()));
            initialComponentParameters.add("");
            initialComponentParameters.add("");
            initialComponentParameters.add("OFF");
            initialComponentParameters.add("4");
            guiArray.add(initialComponentParameters);
            
            int flag=0;
            for(int i=0;i<guiArray.size();i++)
            {
                if(guiArray.getReference(i)[1]=="SAMPLES_PER_BEAT")
                {
                    guiArray.set(i,initialComponentParameters);
                    flag++;
                    break;
                }
            }
            
            if(flag==0)
            {
                guiArray.add(initialComponentParameters);
            }
            
            recalculateBpmFlag=0;
            
            
        }*/
        
        /*fullCode.replaceSection(guiEndPosition.getPosition(),guiEndPosition.getPosition()+,guiCode.getAllContent());
        
        output->setPosition(0);
        output->truncate();
        //output->setNewLineString("\n");
        output->writeText(fullCode.getAllContent(),false,false, nullptr);
        output->flush();
        
        //owner.content=owner.fullCode.getAllContent();
        state.setProperty(ids.patchCode, fullCode.getAllContent(), nullptr);*/
        
        juce::AudioPlayHead* playHead;
        juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;
        double bpm;
        
        playHead = this->getPlayHead();
        playHead->getCurrentPosition (currentPositionInfo);
        bpm = currentPositionInfo.bpm;
        std::cout << plugin->sendInputEvent("SAMPLES_PER_BEAT",(60/bpm)*getSampleRate());
        
        if (plugin != nullptr && ! isSuspended())
            return plugin->processBlock (audio, midi);

        audio.clear();
        midi.clear();
    }

    //==============================================================================
    const juce::String getName() const override                 { return patchName; }
    juce::AudioProcessorEditor* createEditor() override         { return new Editor (*this); }
    bool hasEditor() const override                             { return true; }

    bool acceptsMidi() const override                           { return true; }
    bool producesMidi() const override                          { return false; }
    bool supportsMPE() const override                           { return true; }
    bool isMidiEffect() const override                          { return false; }
    double getTailLengthSeconds() const override                { return plugin != nullptr ? plugin->getTailLengthSeconds() : 0.0; }

    //==============================================================================
    int getNumPrograms() override                               { return 1; }
    int getCurrentProgram() override                            { return 0; }
    void setCurrentProgram (int) override                       {}
    const juce::String getProgramName (int) override            { return {}; }
    void changeProgramName (int, const juce::String&) override  {}
    

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& data) override
    {
        if (plugin != nullptr)
        {
            state.removeAllChildren (nullptr);
            state.addChild (plugin->getUpdatedState(), 0, nullptr);
        }

        juce::MemoryOutputStream out (data, false);
        state.writeToStream (out);
    }

    void setStateInformation (const void* data, int size) override
    {
        
        auto s = juce::ValueTree::readFromData (data, (size_t) size);
        
        //write preset's code into soul file
        if(output->openedOk())
        {
            output->setPosition(0);
            output->truncate();
            //output->setNewLineString("\n");
            output->writeText(s.getProperty(ids.patchCode).toString().toStdString(),false,false, nullptr);
            output->flush();
            
            //write preset's code into CodeWindow
            loadCode();
            
            state.setProperty (ids.patchURL, tempPatch.getFile().getFullPathName(), nullptr);
        }
        
        juce::Array<juce::Array<juce::String>> componentsStorage;
        
        for(int i=0;i<s.getProperty(ids.patchComponents).getArray()->size();i++)
        {
            juce::Array<juce::String> parametersStorage;
            for(int j=0;j<s.getProperty(ids.patchComponents)[i].getArray()->size();j++)
            {
                //DBG("Var Array Size: ");
                //DBG(s.getProperty(ids.patchComponents)[i].getArray()->size());
                parametersStorage.add(s.getProperty(ids.patchComponents)[i].getArray()->getReference(j));
            }
            componentsStorage.add(parametersStorage);
        }
        
        guiArray=componentsStorage;

        if (s.hasType (ids.SOULPatchPlugin))
        {
            state = std::move (s);
            //updatePatchState();
        }
    }
    

    void updatePatchState()
    {
        auto stateID  = state.getProperty (ids.patchID).toString().toStdString();
        auto stateURL = state.getProperty (ids.patchURL).toString().toStdString();

        if (patchInstance != nullptr)
        {
            std::string loadedID, loadedURL;

            if (auto desc = soul::patch::Description::Ptr (patchInstance->getDescription()))
            {
                loadedID = desc->UID;
                loadedURL = desc->URL;
            }

            if (stateID != loadedID || stateURL != loadedURL)
            {
                replaceCurrentPlugin ({});
                patchInstance = nullptr;
            }
        }

        if (patchInstance == nullptr)
            patchInstance = patchLibrary.createPatchInstance (stateURL);

        if (patchInstance != nullptr)
        {
            if (auto desc = soul::patch::Description::Ptr (patchInstance->getDescription()))
            {
                if (std::string_view (desc->UID).empty())
                {
                    replaceCurrentPlugin ({});
                }
                else
                {
                    state.setProperty (ids.patchID, desc->UID, nullptr);

                    if (plugin == nullptr)
                    {
                        auto newPlugin = std::make_unique<soul::patch::SOULPatchAudioProcessor> (patchInstance, getCompilerCache());
                        newPlugin->askHostToReinitialise = [this] { this->reinitialiseSourcePlugin(); };

                        if (state.getNumChildren() != 0)
                            newPlugin->applyNewState (state.getChild (0));

                        newPlugin->setBusesLayout (getBusesLayout());
                        preparePluginToPlayIfPossible (*newPlugin);
                        replaceCurrentPlugin (std::move (newPlugin));
                    }
                    else
                    {
                        if (state.getNumChildren() != 0)
                            plugin->applyNewState (state.getChild (0));
                    }
                }
            }
        }

        updatePatchName();
    }

    //==============================================================================
    struct Editor  : public juce::AudioProcessorEditor,
                     public juce::FileDragAndDropTarget,
                     private juce::Button::Listener
    {
        Editor (EZDSPPlugin& p)  : juce::AudioProcessorEditor (p), owner (p)
        {
            setLookAndFeel (&lookAndFeel);
            //setSize(800, 600);
            //setResizable(true,true);
            refreshContent();
            juce::Font::setDefaultMinimumHorizontalScaleFactor (1.0f);
            
            codeWindow.setSize(400,500);
            codeWindow.setColour(juce::CodeEditorComponent::ColourIds::backgroundColourId, juce::Colours::white);
            codeWindow.setColour(juce::CodeEditorComponent::ColourIds::lineNumberTextId, juce::Colours::black);
            addAndMakeVisible(codeWindow);
            
            runCode.setSize(50,50);
            runCode.setButtonText("Run");
            addAndMakeVisible(runCode);
            
            addGUI.setSize(50,50);
            addGUI.setButtonText("Components");
            addAndMakeVisible(addGUI);
            

            //Editor listens for "run" button to update SOUL file
            runCode.addListener(this);
            addGUI.addListener(this);


            
        }

        ~Editor() override
        {
            owner.editorBeingDeleted (this);
            if (guiWindow){delete guiWindow;}
            setLookAndFeel (nullptr);
        }

        void clearContent()
        {
            setDragOver (false);
            pluginEditor.reset();
            setSize (400, 300);
            repaint();
        }

        void refreshContent()
        {
            clearContent();

            if (owner.plugin != nullptr)
                pluginEditor.reset (owner.plugin->createEditor());

            if (pluginEditor != nullptr)
            {
                addAndMakeVisible (pluginEditor.get());
                childBoundsChanged (nullptr);
            }
        }

        void childBoundsChanged (Component*) override
        {
            if (pluginEditor != nullptr)
            {
                setSize (pluginEditor->getWidth(),
                         pluginEditor->getHeight()+200);
                
                //setSize (300,300);
                
                codeWindow.setBounds(0, getHeight()-200, getWidth(), 150);
                runCode.setBounds(50, getHeight()-40, 50, 30);
                addGUI.setBounds(150, getHeight()-40, 150, 30);
            }
            
        }

        void paint (juce::Graphics& g) override
        {
            auto backgroundColour = getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId);
            g.fillAll (backgroundColour);

            if (pluginEditor == nullptr)
            {
                auto message = owner.patchLibrary.getErrorMessage();

                if (message.empty())
                    message = "Something has gone wrong. Please remove EZDSP and reload";

                g.setColour (backgroundColour.contrasting());
                g.setFont (juce::Font (19.0f, juce::Font::bold));
                g.drawFittedText (message, getLocalBounds().reduced (20), juce::Justification::centred, 5);
            }
        }

        void paintOverChildren (juce::Graphics& g) override
        {
            if (isDragOver)
                g.fillAll (juce::Colours::lightgreen.withAlpha (0.3f));
        }

        bool isInterestedInFileDrag (const juce::StringArray& files) override  { return files.size() == 1 && files[0].endsWith (soul::patch::getManifestSuffix()); }
        void fileDragEnter (const juce::StringArray&, int, int) override       { setDragOver (true); }
        void fileDragExit (const juce::StringArray&) override                  { setDragOver (false); }

        void filesDropped (const juce::StringArray& files, int, int) override
        {
            setDragOver (false);

            if (files.size() == 1)
                owner.setPatchURL (files[0].toStdString());
                
        }

        void setDragOver (bool b)
        {
            if (isDragOver != b)
            {
                isDragOver = b;
                repaint();
            }
        }
        
        void buttonClicked(juce::Button* button) override
        {
            if (button == &runCode && owner.output->openedOk())
            {
                
                owner.fullCode.replaceSection(owner.dspStartPosition.getPosition(),owner.dspEndPosition.getPosition(),owner.dspCode.getAllContent());
                
                juce::String tempGuiCode="";
                
                for(int i=0; i< owner.guiArray.size();i++)
                {
                    if(owner.guiArray[i][0]== "VARIABLE")
                    {
                        tempGuiCode+= owner.guiArray[i][2] + " " + owner.guiArray[i][1] + " = " + owner.guiArray[i][5] + ";\n";
                    }
                    
                    else if(owner.guiArray[i][0]== "SLIDER")
                    {
                        tempGuiCode+= "input stream float " + owner.guiArray[i][1] + " [[ name: \"" +  owner.guiArray[i][1] + "\", min: " + owner.guiArray[i][3] +", max: " + owner.guiArray[i][4] + ", init:  " + owner.guiArray[i][5] + ", step: " + owner.guiArray[i][6] + " ]];\n";
                    }
                    else if(owner.guiArray[i][0]== "BUFFER")
                    {
                        tempGuiCode+= owner.guiArray[i][2] + "[" + owner.guiArray[i][7] + "] " + owner.guiArray[i][3] + ";\n" + "wrap<" + owner.guiArray[i][7] + "> " + owner.guiArray[i][3] + "Index;\n";
                    }
                    
                    else if(owner.guiArray[i][0]== "BUTTON")
                    {
                        tempGuiCode+= "input stream float " + owner.guiArray[i][1] + " [[ name: \"" +  owner.guiArray[i][1] + "\", boolean ]];\n";
                    }
                }
                
                owner.guiCode.replaceAllContent(tempGuiCode);
                owner.fullCode.replaceSection(owner.guiStartPosition.getPosition(),owner.guiEndPosition.getPosition(),owner.guiCode.getAllContent());
                
                owner.output->setPosition(0);
                owner.output->truncate();
                owner.output->writeText(owner.fullCode.getAllContent(),false,false, nullptr);
                owner.output->flush();
                owner.state.setProperty(owner.ids.patchCode, owner.fullCode.getAllContent(), nullptr);
                
                //store array of components in vars and add to ValueTree
                juce::Array<juce::var> componentsStorage;
                for(int i=0;i<owner.guiArray.size();i++)
                {
                    juce::Array<juce::var> parametersStorage;
                    for(int j=0;j<owner.guiArray[i].size();j++)
                    {
                        //DBG("String Array Size: ");
                        //DBG(owner.guiArray[i].size());
                        parametersStorage.add(owner.guiArray[i][j]);
                    }
                    componentsStorage.add(parametersStorage);
                }
                
                juce::var tempStorage= componentsStorage;
                owner.state.setProperty(owner.ids.patchComponents, tempStorage, nullptr);
                //owner.updatePatchState();
            }
            
            if (button == &addGUI)
            {
                if(guiWindow == nullptr)
                {
                    guiWindow = new guiCreator("guiComponents", &owner.guiArray, &runCode);
                    guiWindow->setUsingNativeTitleBar(true);
                    //guiWindow->setContentOwned(new guiCreator("guiComponents"), true);
                    guiWindow->centreWithSize(pluginEditor->getWidth(), pluginEditor->getHeight());
                    guiWindow->setAlwaysOnTop(true);
                    guiWindow->setVisible(true);
                }
                else
                {
                    guiWindow->toFront(true);
                }
            }
        }

        EZDSPPlugin& owner;
        std::unique_ptr<AudioProcessorEditor> pluginEditor;
        juce::LookAndFeel_V4 lookAndFeel;
        bool isDragOver = false;
        
        juce::EzdspTokeniser ezdspTokenizer;
        juce::CodeEditorComponent codeWindow{owner.dspCode, &ezdspTokenizer };
        juce::Component::SafePointer<juce::TopLevelWindow> guiWindow;
        juce::TextButton runCode,addGUI;
        

    };
    
    //public: (audio processor public variables)
    juce::File presetCode= juce::File ("/Library/Application Support/EZDSP/Default.soul");
    juce::File presetPatch= juce::File("/Library/Application Support/EZDSP/Default.soulpatch");
    juce::TemporaryFile tempCode= juce::TemporaryFile (presetCode);
    juce::TemporaryFile tempPatch= juce::TemporaryFile (presetPatch);
    
    std::unique_ptr<juce::FileOutputStream> output;
    std::unique_ptr<juce::FileInputStream> input;
    
    std::unique_ptr<juce::FileOutputStream> outputPatch;
    std::unique_ptr<juce::FileInputStream> inputPatch;
    
    juce::CodeDocument fullCode ,guiCode, dspCode;
    juce::CodeDocument::Position dspStartPosition, dspEndPosition, guiStartPosition, guiEndPosition;

private:
    //==============================================================================
    PatchLibrary patchLibrary;
    juce::String patchName;
    soul::patch::PatchInstance::Ptr patchInstance;
    std::unique_ptr<soul::patch::SOULPatchAudioProcessor> plugin;
    juce::ValueTree state;
    soul::patch::CompilerCache::Ptr compilerCache;
    juce::Array<juce::Array <juce::String>> guiArray;
    
    int recalculateBpmFlag=0;
    

    struct IDs
    {
        const juce::Identifier SOULPatchPlugin   { "SOULPatchPlugin" },
                               patchURL          { "patchURL" },
                               patchID           { "patchID" },
                               patchCode         {"patchCode"},
                               patchComponents   {"patchComponents"};
    };

    IDs ids;

    soul::patch::CompilerCache::Ptr getCompilerCache()
    {
        constexpr uint32_t maxNumCacheFiles = 200;

        if (compilerCache == nullptr)
        {
           #if JUCE_MAC
            auto tempFolder = juce::File ("~/Library/Caches");
           #else
            auto tempFolder = juce::File::getSpecialLocation (juce::File::SpecialLocationType::tempDirectory);
           #endif

            auto cacheFolder = tempFolder.getChildFile ("dev.soul.SOULPlugin").getChildFile ("Cache");

            if (cacheFolder.createDirectory())
                compilerCache = soul::patch::CompilerCache::Ptr (new soul::patch::CompilerCacheFolder (cacheFolder, maxNumCacheFiles));
        }

        return compilerCache;
    }

    void preparePluginToPlayIfPossible (soul::patch::SOULPatchAudioProcessor& p)
    {
        auto rate = getSampleRate();
        auto size = getBlockSize();

        if (rate > 0 && size > 0)
            p.prepareToPlay (rate, size);
    }

    void reinitialiseSourcePlugin()
    {
        suspendProcessing (true);

        if (plugin != nullptr)
        {
            plugin->setBusesLayout (getBusesLayout());
            plugin->reinitialise();
            preparePluginToPlayIfPossible (*plugin);
        }

        updateHostDisplay();
        suspendProcessing (false);

        if (auto ed = dynamic_cast<Editor*> (getActiveEditor()))
            ed->refreshContent();
    }

    void replaceCurrentPlugin (std::unique_ptr<soul::patch::SOULPatchAudioProcessor> newPlugin)
    {
        if (newPlugin.get() != plugin.get())
        {
            if (auto ed = dynamic_cast<Editor*> (getActiveEditor()))
                ed->clearContent();

            suspendProcessing (true);
            std::swap (plugin, newPlugin);
            suspendProcessing (false);

            if (auto ed = dynamic_cast<Editor*> (getActiveEditor()))
                ed->refreshContent();
        }
    }

    void checkForSiblingPatch()
    {
        auto pluginDLL = juce::File::getSpecialLocation (juce::File::SpecialLocationType::currentApplicationFile);
        auto siblingPatches = pluginDLL.getParentDirectory().findChildFiles (juce::File::findFiles, false, "*.soulpatch");

        if (siblingPatches.size() == 1)
            setPatchURL (siblingPatches.getFirst().getFullPathName().toStdString());
    }

    void updatePatchName()
    {
        if (patchInstance != nullptr)
        {
            if (auto desc = soul::patch::Description::Ptr (patchInstance->getDescription()))
            {
                if (std::string_view (desc->name).empty())
                {
                    patchName = desc->name;
                    return;
                }
            }
        }

        patchName = "SOUL Patch Loader";
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EZDSPPlugin)
};


//==============================================================================
/** This helper class can be used as the template class for EZDSPPlugin to make it
    find and load the SOUL_PatchLoader DLL as its JIT engine.
*/
struct PatchLibraryDLL
{
    PatchLibraryDLL()
    {
        library->ensureLibraryLoaded (lookForSOULPatchDLL().toStdString());
    }

    std::string getErrorMessage()
    {
        if (library->library == nullptr)
            return std::string ("Couldn't find or load ") + soul::patch::SOULPatchLibrary::getLibraryFileName();

        return {};
    }

    soul::patch::PatchInstance::Ptr createPatchInstance (const std::string& url)
    {
        if (library->library != nullptr)
            return soul::patch::PatchInstance::Ptr (library->library->createPatchFromFileBundle (url.c_str()));

        return {};
    }

private:
    //==============================================================================
    static juce::String lookForSOULPatchDLL()
    {
        auto dllName = soul::patch::SOULPatchLibrary::getLibraryFileName();

        auto pluginDLL = juce::File::getSpecialLocation (juce::File::SpecialLocationType::currentApplicationFile);
        auto pluginSibling = pluginDLL.getSiblingFile (dllName);

        if (pluginSibling.exists())
            return pluginSibling.getFullPathName();

       #if JUCE_MAC
        auto insideBundle = pluginDLL.getChildFile ("Contents/Resources").getChildFile (dllName);

        if (insideBundle.exists())
            return insideBundle.getFullPathName();
       #endif

        auto inAppData = juce::File::getSpecialLocation (juce::File::SpecialLocationType::userApplicationDataDirectory)
                            .getChildFile ("SOUL").getChildFile (dllName);

        if (inAppData.exists())
            return inAppData.getFullPathName();

        return dllName;
    }

    struct SharedPatchLibraryHolder
    {
        SharedPatchLibraryHolder() = default;

        void ensureLibraryLoaded (const std::string& patchLoaderLibraryPath)
        {
            if (library == nullptr)
            {
                library = std::make_unique<soul::patch::SOULPatchLibrary> (patchLoaderLibraryPath.c_str());

                if (library->loadedSuccessfully())
                    loadedPath = patchLoaderLibraryPath;
                else
                    library.reset();
            }
            else
            {
                // This class isn't sophisticated enough to be able to load multiple
                // DLLs from different locations at the same time
                jassert (loadedPath == patchLoaderLibraryPath);
            }
        }

        std::unique_ptr<soul::patch::SOULPatchLibrary> library;
        std::string loadedPath;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SharedPatchLibraryHolder)
    };

    juce::SharedResourcePointer<SharedPatchLibraryHolder> library;
};


} // namespace patch
} // namespace soul

