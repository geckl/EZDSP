/*
  ==============================================================================

    renamableParameter.h
    Created: 24 Aug 2022 8:23:36pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

class Renameable : public juce::AudioParameterFloat
{
public:
    
    //using juce::AudioParameterFloat::AudioParameterFloat;
    
    Renameable(const juce::ParameterID &parameterID, const juce::String &parameterName, juce::NormalisableRange< float > normalisableRange, float defaultValue, const juce::AudioParameterFloatAttributes &attributes={})
    : AudioParameterFloat {parameterID, parameterName, normalisableRange, defaultValue, attributes}
    {
        setName(parameterName);
    }

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

private:
    juce::String name;
};

