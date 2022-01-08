/*
  ==============================================================================

    ezdspCodeTokenizer.cpp
    Created: 7 Jan 2022 4:29:24pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#include "ezdspCodeTokenizer.h"


namespace juce
{

struct EzdspTokeniserFunctions
{
    static bool isReservedKeyword (String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char* const keywords2Char[] =
            { "do", "if", nullptr };

        static const char* const keywords3Char[] =
            { "for", "int", "let", "var", nullptr };

        static const char* const keywords4Char[] =
            { "bool", "else", "loop", "true", "void", "wrap", nullptr };

        static const char* const keywords5Char[] =
            {  "break", "clamp", "const", "event", "false", "float", "graph", "input", "int32", "int64", "using", "while", nullptr };

        static const char* const keywords6Char[] =
            { "output", "return", "string", "struct", nullptr};

        static const char* const keywordsOther[] =
            { "connection", "continue", "external", "float32", "float64", "namespace", "processor", nullptr };

        const char* const* k;

        switch (tokenLength)
        {
            case 2:   k = keywords2Char; break;
            case 3:   k = keywords3Char; break;
            case 4:   k = keywords4Char; break;
            case 5:   k = keywords5Char; break;
            case 6:   k = keywords6Char; break;

            default:
                if (tokenLength < 2 || tokenLength > 16)
                    return false;

                k = keywordsOther;
                break;
        }

        for (int i = 0; k[i] != nullptr; ++i)
            if (token.compare (CharPointer_ASCII (k[i])) == 0)
                return true;

        return false;
    }

    template <typename Iterator>
    static int parseIdentifier (Iterator& source) noexcept
    {
        int tokenLength = 0;
        String::CharPointerType::CharType possibleIdentifier[100];
        String::CharPointerType possible (possibleIdentifier);

        while (CppTokeniserFunctions::isIdentifierBody (source.peekNextChar()))
        {
            auto c = source.nextChar();

            if (tokenLength < 20)
                possible.write (c);

            ++tokenLength;
        }

        if (tokenLength > 1 && tokenLength <= 16)
        {
            possible.writeNull();

            if (isReservedKeyword (String::CharPointerType (possibleIdentifier), tokenLength))
                return EzdspTokeniser::tokenType_keyword;
        }

        return EzdspTokeniser::tokenType_identifier;
    }

    template <typename Iterator>
    static int readNextToken (Iterator& source)
    {
        source.skipWhitespace();

        auto firstChar = source.peekNextChar();

        switch (firstChar)
        {
        case 0:
            break;

        case '0':   case '1':   case '2':   case '3':   case '4':
        case '5':   case '6':   case '7':   case '8':   case '9':
        case '.':
        {
            auto result = CppTokeniserFunctions::parseNumber (source);

            if (result == EzdspTokeniser::tokenType_error)
            {
                source.skip();

                if (firstChar == '.')
                    return EzdspTokeniser::tokenType_punctuation;
            }

            return result;
        }

        case ',':
        case ';':
        case ':':
            source.skip();
            return EzdspTokeniser::tokenType_punctuation;

        case '(':   case ')':
        case '{':   case '}':
        case '[':   case ']':
            source.skip();
            return EzdspTokeniser::tokenType_bracket;

        case '"':
        case '\'':
            CppTokeniserFunctions::skipQuotedString (source);
            return EzdspTokeniser::tokenType_string;

        case '+':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '+', '=');
            return EzdspTokeniser::tokenType_operator;

        case '-':
        {
            source.skip();
            auto result = CppTokeniserFunctions::parseNumber (source);

            if (result == EzdspTokeniser::tokenType_error)
            {
                CppTokeniserFunctions::skipIfNextCharMatches (source, '-', '=');
                return EzdspTokeniser::tokenType_operator;
            }

            return result;
        }

        case '*':   case '%':
        case '=':   case '!':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
            return EzdspTokeniser::tokenType_operator;

        case '?':
        case '~':
            source.skip();
            return EzdspTokeniser::tokenType_operator;

        case '<':   case '>':
        case '|':   case '&':   case '^':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
            CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
            return EzdspTokeniser::tokenType_operator;
                
        case '/':
            {
                source.skip();
                if (source.peekNextChar() == '/')
                {
                    source.skipToEndOfLine();
                    return EzdspTokeniser::tokenType_comment;
                }
                break;
            }

        default:
            if (CppTokeniserFunctions::isIdentifierStart (firstChar))
                return parseIdentifier (source);

            source.skip();
            break;
        }

        return EzdspTokeniser::tokenType_error;
    }
};

//==============================================================================
EzdspTokeniser::EzdspTokeniser() {}
EzdspTokeniser::~EzdspTokeniser() {}

int EzdspTokeniser::readNextToken (CodeDocument::Iterator& source)
{
    return EzdspTokeniserFunctions::readNextToken (source);
}

CodeEditorComponent::ColourScheme EzdspTokeniser::getDefaultColourScheme()
{
    static const CodeEditorComponent::ColourScheme::TokenType types[] =
    {
        { "Error",          Colour (0xffcc0000) },
        { "Comment",        Colour (0xff3c3c3c) },
        { "Keyword",        Colour (0xff0000cc) },
        { "Operator",       Colour (0xff225500) },
        { "Identifier",     Colour (0xff000000) },
        { "Integer",        Colour (0xff880000) },
        { "Float",          Colour (0xff885500) },
        { "String",         Colour (0xff990099) },
        { "Bracket",        Colour (0xff000055) },
        { "Punctuation",    Colour (0xff004400) }
    };

    CodeEditorComponent::ColourScheme cs;

    for (auto& t : types)
        cs.set (t.name, Colour (t.colour));

    return cs;
}

} // namespace juce
