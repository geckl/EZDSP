/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== Default.soul ==================
static const unsigned char temp_binary_data_0[] =
"/**\n"
"    This is an auto-generated SOUL patch template.\n"
"    This example code simply performs a simple gain between its input\n"
"    and output. Now it's your turn to build this up into a real effect!\n"
"*/\n"
"processor Test [[main]]\n"
"{\n"
"    input stream float audioIn;\n"
"    input event soul::timeline::Tempo tempoIn;\n"
"    input event soul::timeline::Position positionIn;\n"
"    input event soul::timeline::TimeSignature timesignatureIn;\n"
"    output stream float audioOut;\n"
"\n"
"\n"
"//BEGINGUI\n"
"\n"
"    input stream float GAIN  [[ name: \"GAIN\", min: -60.0, max: 10.0, init: 0, step: 0.1, slewRate: 200.0 ]];\n"
"    int SAMPLERATE = 0;\n"
"\n"
"//ENDGUI\n"
"\n"
"    event tempoIn (soul::timeline::Tempo t)\n"
"    {\n"
"    BPM = t.bpm;\n"
"    }\n"
"\n"
"    event positionIn (soul::timeline::Position p)\n"
"    {\n"
"    CURRENTSAMPLE = p.currentFrame;\n"
"    }\n"
"\n"
"    event timesignatureIn (soul::timeline::TimeSignature s)\n"
"    {\n"
"    NUMERATOR = s.numerator;\n"
"    DENOMINATOR = s.denominator;\n"
"    }\n"
"\n"
"    float BPM, SAMPLESPERBEAT;\n"
"    int64 CURRENTSAMPLE, NUMERATOR, DENOMINATOR;\n"
"\n"
"    void run()\n"
"    {\n"
"        loop\n"
"        {\n"
"\n"
"\n"
"//BEGINDSP\n"
"\n"
"let masterGain = soul::dBtoGain (GAIN);\n"
"\n"
"audioOut << audioIn * masterGain;\n"
"\n"
"//ENDDSP\n"
"\n"
"\n"
"            advance();\n"
"        }\n"
"    }\n"
"}";

const char* Default_soul = (const char*) temp_binary_data_0;

//================== Default.soulpatch ==================
static const unsigned char temp_binary_data_1[] =
"{\n"
"    \"soulPatchV1\":\n"
"    {\n"
"        \"ID\":               \"ezdsp.default\",\n"
"        \"version\":          \"1.0\",\n"
"        \"name\":             \"Default\",\n"
"        \"description\":      \"EZDSP Default PAtch\",\n"
"        \"category\":         \"fx\",\n"
"        \"manufacturer\":     \"Squatch Sounds\",\n"
"        \"isInstrument\":     false,\n"
"        \n"
"        \"source\":           \"Default.soul\"\n"
"    }\n"
"}\n";

const char* Default_soulpatch = (const char*) temp_binary_data_1;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes);
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0xe308ed51:  numBytes = 1187; return Default_soul;
        case 0xbf13dd97:  numBytes = 372; return Default_soulpatch;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "Default_soul",
    "Default_soulpatch"
};

const char* originalFilenames[] =
{
    "Default.soul",
    "Default.soulpatch"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
    {
        if (namedResourceList[i] == resourceNameUTF8)
            return originalFilenames[i];
    }

    return nullptr;
}

}
