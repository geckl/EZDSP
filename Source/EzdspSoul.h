/*
  ==============================================================================

    EzdspSoul.h
    Created: 11 Feb 2021 10:50:00pm
    Author:  Garrett Eckl

  ==============================================================================

*/

#pragma once


#ifndef JUCE_AUDIO_PROCESSORS_H_INCLUDED
 #error "this header is designed to be included in JUCE projects that contain the juce_audio_processors module"
#endif


#include "../SOUL-master/include/soul/soul_patch.h"
#include "../SOUL-master/include/soul/patch/helper_classes/soul_patch_Utilities.h"
#include "../SOUL-master/include/soul/patch/helper_classes/soul_patch_CompilerCacheFolder.h"
//#include "../SOUL-master/include/soul/patch/helper_classes/soul_patch_AudioProcessor.h"
#include "../SOUL-overrides/soul_patch_AudioProcessor.h"

#include "../JuceLibraryCode/BinaryData.h"
#include "guiCreator.h"
#include "EzdspHelp.h"
#include "Utils/EzdspCodeTokenizer.h"
#include "Utils/Vectors.h"


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

        
        //presetCode.copyFileTo(tempCode.getFile());
        //presetPatch.copyFileTo(tempPatch.getFile());
        
        //Load default EZDSP patch from binary data into temp files
        tempCode.getFile().appendData(BinaryData::Default_soul, BinaryData::Default_soulSize);
        tempPatch.getFile().appendData(BinaryData::Default_soulpatch, BinaryData::Default_soulpatchSize);
        
        //set new .soulpatch file to link up with new .soul file
        juce::var parsedJson= juce::JSON::parse(tempPatch.getFile());
        auto obj(parsedJson["soulPatchV1"].getDynamicObject());
        //auto answer=obj->getProperties().size();
        obj->setProperty("source",tempCode.getFile().getRelativePathFrom(tempPatch.getFile()));
        tempPatch.getFile().replaceWithText(juce::JSON::toString(parsedJson));
        
        
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
        initialComponentParameters.add("1");
        initialComponentParameters.add("GAIN");
        guiArray.add(initialComponentParameters);
        
        initialComponentParameters.clear();
        
        initialComponentParameters.add("NUMBER");
        initialComponentParameters.add("SAMPLERATE");
        initialComponentParameters.add("int");
        initialComponentParameters.add("");
        initialComponentParameters.add("");
        initialComponentParameters.add("0");
        initialComponentParameters.add("");
        initialComponentParameters.add("");
        initialComponentParameters.add("OFF");
        initialComponentParameters.add("4");
        initialComponentParameters.add("SAMPLERATE");
        guiArray.add(initialComponentParameters);

        //Load soulpatch properties into ValueTree
        state = juce::ValueTree (ids.SOULPatchPlugin);
        state.setProperty (ids.patchURL, tempPatch.getFile().getFullPathName(), nullptr);
        
        
        juce::String initialDSP = "let masterGain = soul::dBtoGain (GAIN);\n\naudioOut << audioIn * masterGain;";
        
        
        dspCode.replaceAllContent(initialDSP);
        
        updatePatch();
        updatePatchState();
    }

    ~EZDSPPlugin() override
    {
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
    
    void updatePatch()
    {
        const juce::String guiCode = guiArrayToCode(guiArray);
        tempCode.getFile().replaceWithText(combineCode(guiCode, dspCode.getAllContent()),false,false,nullptr);
        
        usedWords.clear();
        
        //store array of components in vars and add to ValueTree
        juce::Array<juce::var> componentsStorage;
        for(int i=0;i<guiArray.size();i++)
        {
            //add component names to vector of in-use variable names
            usedWords.push_back(guiArray[i][10].toStdString());
            
            juce::Array<juce::var> parametersStorage;
            for(int j=0;j<guiArray[i].size();j++)
            {
                //DBG("String Array Size: ");
                //DBG(owner.guiArray[i].size());
                parametersStorage.add(guiArray[i][j]);
            }
            componentsStorage.add(parametersStorage);
        }
        
        juce::var tempGUI= componentsStorage;
        state.setProperty(ids.patchComponents, tempGUI, nullptr);
        juce::var tempDSP = dspCode.getAllContent();
        state.setProperty(ids.patchDSP, tempDSP, nullptr);
    }
    
    juce::String combineCode (juce::String guiCode, juce::String dspCode)
    {
        juce::String code1 = R"(
        processor Test [[main]]
        {
            input stream float32 audioIn;
            input event soul::timeline::Tempo tempoIn;
            input event soul::timeline::Position positionIn;
            input event soul::timeline::TimeSignature timesignatureIn;
            output stream float32 audioOut;
        
        )";
        
        juce::String code2 = R"(
        
        event tempoIn (soul::timeline::Tempo t)
        {
        BPM = t.bpm;
        }

        event positionIn (soul::timeline::Position p)
        {
        CURRENTSAMPLE = p.currentFrame;
        }

        event timesignatureIn (soul::timeline::TimeSignature s)
        {
        NUMERATOR = s.numerator;
        DENOMINATOR = s.denominator;
        }

        float BPM, SAMPLESPERBEAT;
        int64 CURRENTSAMPLE, NUMERATOR, DENOMINATOR;

        void run()
        {
            loop
            {

        )";
        
        juce::String code3 = R"(
        
            advance();
            }
        }
        }

        )";
        
        juce::String combinedCode = code1 + guiCode + code2 + dspCode + code3;
        return combinedCode;
    }
    
    juce::String guiArrayToCode(juce::Array<juce::Array <juce::String>> guiArray)
    {
        juce::String guiCode="";
        
        for(int i=0; i< guiArray.size();i++)
        {
            if(guiArray[i][0]== "NUMBER")
            {
                guiCode+= guiArray[i][2] + " " + guiArray[i][10] + " = " + guiArray[i][5] + ";\n";
            }
            
            else if(guiArray[i][0]== "SLIDER")
            {
                guiCode+= "input stream float " + guiArray[i][10] + " [[ name: \"" +  guiArray[i][10] + "\", min: " + guiArray[i][3] +", max: " + guiArray[i][4] + ", init:  " + guiArray[i][5] + ", step: " + guiArray[i][6] + " ]];\n";
            }
            else if(guiArray[i][0]== "BUFFER")
            {
                guiCode+= guiArray[i][2] + "[" + guiArray[i][7] + "] " + guiArray[i][10] + ";\n" + "wrap<" + guiArray[i][7] + "> " + guiArray[i][10] + "Index;\n";
            }
            
            else if(guiArray[i][0]== "BUTTON")
            {
                guiCode+= "input stream float " + guiArray[i][10] + " [[ name: \"" +  guiArray[i][10] + "\", boolean ]];\n";
            }
        }
        return guiCode;
    }
    
    void updateSampleRateVariable(double sampleRate)
    {
        //update sample rate global variable
        for(int i=0;i<guiArray.size();i++)
        {
            if(guiArray.getReference(i)[1]=="SAMPLERATE")
            {
                guiArray.getReference(i).set(5,juce::String(sampleRate));
                break;
            }
        }
    }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        updateSampleRateVariable(sampleRate);
        
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
    
    void processorLayoutsChanged() override
    {
        //DBG("Layouts changed!");
        if(plugin != nullptr)
        {
            plugin->setBusesLayout(getBusesLayout());
        }
    }

    void processBlock (juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) override
    {
        
        //juce::var bpm;
        
        currentPlayHead = this->getPlayHead();
        if (plugin != nullptr)
        {
            plugin->setPlayHead(currentPlayHead);
        }
        //currentPlayHead->getCurrentPosition (currentPositionInfo);
        //bpm = currentPositionInfo.bpm;
        //std::cout << plugin->sendInputEvent("SAMPLES_PER_BEAT",(60/bpm)*getSampleRate());
        //plugin->getPatchPlayer()->applyNewTempo(bpm);
        
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
        
        if (s.hasType (ids.SOULPatchPlugin))
        {
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
            
            updateSampleRateVariable(getSampleRate());
            
            juce::String tempDSP = s.getProperty(ids.patchDSP);
            dspCode.replaceAllContent(tempDSP);
            
            updatePatch();
            //state = std::move (s);
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
                     private juce::Button::Listener
    {
        Editor (EZDSPPlugin& p)  : juce::AudioProcessorEditor (p), owner (p)
        {
            setLookAndFeel (&lookAndFeel);
            //setSize(800, 600);
            //setResizable(true,true);
            refreshContent();
            juce::Font::setDefaultMinimumHorizontalScaleFactor (1.0f);
            
            codeWindow.setColour(juce::CodeEditorComponent::ColourIds::backgroundColourId, juce::Colours::white);
            codeWindow.setColour(juce::CodeEditorComponent::ColourIds::lineNumberTextId, juce::Colours::black);
            codeWindow.setWantsKeyboardFocus(true);
            codeWindow.setOpaque(true);
            addAndMakeVisible(codeWindow);
            
            runCode.setButtonText("Run");
            addAndMakeVisible(runCode);
            runCode.addListener(this);
            
            addGUI.setButtonText("Components");
            addAndMakeVisible(addGUI);
            addGUI.addListener(this);
            
            expandText.setButtonText("Text Editor");
            addAndMakeVisible(expandText);
            expandText.addListener(this);
            
            getHelp.setButtonText("Help");
            addAndMakeVisible(getHelp);
            getHelp.addListener(this);
            
            auto logo= juce::ImageCache::getFromMemory(BinaryData::EZDSPLogo_png, BinaryData::EZDSPLogo_pngSize);
                
            if(! logo.isNull())
            {
                logoComponent.setImage(logo,64);
            }
            else
            {
                jassert(! logo.isNull());
            }
                
                addAndMakeVisible(logoComponent);
        }

        ~Editor() override
        {
            owner.editorBeingDeleted (this);
            if (guiWindow){delete guiWindow;}
            if (helpWindow){delete helpWindow;}
            setLookAndFeel (nullptr);
        }

        void clearContent()
        {
            //setDragOver (false);
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
                juce::Rectangle<int> r = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
                //int x = r.getWidth();
                int y = r.getHeight();
                
                pluginEditor->setSize(600, int(y*.5));
                
                setSize (pluginEditor->getWidth(),
                         pluginEditor->getHeight()+250);
                
                //setSize (300,300);
                
                codeWindow.setBounds(0, getHeight()-250, getWidth(), 150);
                runCode.setBounds(15, getHeight()-75, 50, 30);
                addGUI.setBounds(80, getHeight()-75, 125, 30);
                expandText.setBounds(395, getHeight()-75, 125, 30);
                getHelp.setBounds(535, getHeight()-75, 50, 30);
                logoComponent.setBounds(220, getHeight()-95, 160, 90);
                
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
        
        void buttonClicked(juce::Button* button) override
        {
            if (button == &runCode && owner.tempCode.getFile().exists())
            {
                owner.updatePatch();
            }
            
            else if (button == &addGUI)
            {
                if(guiWindow == nullptr)
                {
                    guiWindow = new guiCreator("guiComponents", &owner.guiArray, &runCode);
                    guiWindow->setUsingNativeTitleBar(true);
                    guiWindow->centreWithSize(pluginEditor->getWidth(), pluginEditor->getHeight());
                    guiWindow->setAlwaysOnTop(true);
                    guiWindow->setVisible(true);
                }
                else
                {
                    guiWindow->toFront(true);
                }
            }
            
            else if (button == &getHelp)
            {
                if(helpWindow == nullptr)
                {
                    DBG("Open Help Window");
                    helpWindow = new EzdspHelp("EZDSP Help", getWidth(), getHeight());
                }
                else
                {
                    helpWindow->toFront(true);
                }
                
            }
            
            else if(button == &expandText)
            {
                if(codeWindow.isOnDesktop())
                {
                    codeWindow.removeFromDesktop();
                    addAndMakeVisible(codeWindow);
                    childBoundsChanged(&codeWindow);
                }
                else
                {
                    codeWindow.addToDesktop(juce::ComponentPeer::StyleFlags::windowIsResizable | juce::ComponentPeer::StyleFlags::windowHasTitleBar);
                    codeWindow.setAlwaysOnTop(true);
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
        juce::Component::SafePointer<juce::DocumentWindow> helpWindow;
        juce::TextButton runCode,addGUI,getHelp, expandText;
        juce::ImageComponent logoComponent;
    };
    
    //public: (audio processor public variables)
    
    juce::TemporaryFile tempCode = juce::TemporaryFile(".soul");
    juce::TemporaryFile tempPatch = juce::TemporaryFile(".soulpatch");
    
    
    std::unique_ptr<juce::FileOutputStream> outputPatch;
    std::unique_ptr<juce::FileInputStream> inputPatch;
    
    juce::CodeDocument dspCode;
    
    juce::AudioPlayHead* currentPlayHead;
    juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;

private:
    //==============================================================================
    PatchLibrary patchLibrary;
    juce::String patchName;
    soul::patch::PatchInstance::Ptr patchInstance;
    std::unique_ptr<soul::patch::SOULPatchAudioProcessor> plugin;
    juce::ValueTree state;
    soul::patch::CompilerCache::Ptr compilerCache;
    juce::Array<juce::Array <juce::String>> guiArray;

    struct IDs
    {
        const juce::Identifier SOULPatchPlugin   { "SOULPatchPlugin" },
                               patchURL          { "patchURL" },
                               patchID           { "patchID" },
                               patchDSP         {"patchDSP"},
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

