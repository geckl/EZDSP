/*
  ==============================================================================

    renamableParameter.h
    Created: 24 Aug 2022 8:23:36pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

template<typename Parameter>
class Renameable : public Parameter
{
public:
    template<typename... Args>
    Renameable (Args&&... args) : Parameter (std::forward<Args>(args)...) {}

    juce::String getName (int maximumStringLength) const override
    {
        return name.substring (0, maximumStringLength);
    }

    void setName (const juce::String& newName)
    {
        name = newName;
    }

    void setNameNotifyingHost (const juce::String& newName, juce::AudioProcessor& processor)
    {
        const auto details = juce::AudioProcessorListener::ChangeDetails{}.withParameterInfoChanged (true);

        setName (newName);
        processor.updateHostDisplay (details);
    }

    bool isMetaParameter() const override { return true; }
    bool isOrientationInverted() const override { return true; }
    
private:
    juce::String name;
};
