/*
  ==============================================================================

    ezdspCodeTokenizer.h
    Created: 7 Jan 2022 4:29:24pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#include <JuceHeader.h>

namespace juce
{

//==============================================================================
/**
    @tags{GUI}
*/
class JUCE_API  EzdspTokeniser   : public CodeTokeniser
{
public:
    //==============================================================================
    EzdspTokeniser();
    ~EzdspTokeniser() override;

    //==============================================================================
    int readNextToken (CodeDocument::Iterator&) override;
    CodeEditorComponent::ColourScheme getDefaultColourScheme() override;

    /** The token values returned by this tokeniser. */
    enum TokenType
    {
        tokenType_error = 0,
        tokenType_comment,
        tokenType_keyword,
        tokenType_operator,
        tokenType_identifier,
        tokenType_integer,
        tokenType_float,
        tokenType_string,
        tokenType_bracket,
        tokenType_punctuation
    };

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EzdspTokeniser)
};

} // namespace juce
