# EZDSP Documentation

The EZDSP interface consists of two main elements: The DSP Code Editor and the Components Window. Understanding how these elements interact is key to getting your first plug-in up and running.


## The DSP Code Editor

This is where you will write your DSP code. This code exists within a loop, meaning that it will be executed iteratively for each incoming sample of audio.

EZDSP is powered by the SOUL programming language. More info on the SOUL language syntax can be found here: [SOUL Language Guide](https://github.com/soul-lang/SOUL/blob/master/docs/SOUL_Language.md)

### SOUL Functions:

All SOUL functions and operators can be accessed from within the code editor. A full list of available functions can be found here: [SOUL Library Reference](https://soul-lang.github.io/SOUL/docs/soul_library.html)

While most SOUL functions require the use of the *soul::* namespace, a number of lower level functions can be called directly:

#### Arithmetic:

<mark>abs()</mark> <mark>sqrt()</mark> <mark>pow()</mark> <mark>exp()</mark> <mark>log()</mark> <mark>log10()</mark> <mark>floor()</mark> <mark>ceil()</mark> <mark>fmod(numer, denom)</mark> <mark>remainder(numer, denom)</mark>

#### Comparison and ranges:

<mark>min(v1, v2)</mark> <mark>max(v1, v2)</mark> <mark>clamp(value, low, high)</mark> <mark>wrap(v, max)</mark>

#### Trigonometry:

<mark>sin()</mark> <mark>cos()</mark> <mark>tan()</mark> <mark>acos()</mark> <mark>asin()</mark> <mark>atan()</mark> <mark>atan2()</mark> <mark>sinh()</mark> <mark>cosh()</mark> <mark>tanh()</mark> <mark>asinh()</mark> <mark>acosh()</mark> <mark>atanh()</mark>

### Global Variables:

These variables reflect important information about your current DAW Project:

<mark>BPM</mark> - The current tempo in beats per minute
<mark>CURRENTSAMPLE</mark> - The current sample number starting from the beginning of the timeline
<mark>SAMPLERATE</mark> - The current sample rate in hertz
<mark>SAMPLESPERBEAT</mark> - The number of samples per quarter note, given the current tempo and sample rate
<mark>NUMERATOR</mark> - The current time signature numerator (the number of beats per measure)
<mark>DENOMINATOR</mark> - The current time signature denominator (the beat value)

Additionally, the follow global constants are provided:

<mark>pi</mark> - \pi
<mark>twoPi</mark> - 2\pi
<mark>nan</mark> - Not a Number
<mark>inf</mark> - Infinity
<mark>processor.period</mark> - the duration in seconds of one frame
<mark>processor.session</mark> - a unique int32 which is different each time the program runs.


### Local Variables

Local variables declared within the DSP Code Editor will be reinitialized for each sample by default. To circumvent this, users can create inner loops to process chunks of samples at a time. For example:

```
let masterGain = pow (10.0f, GAIN * 0.05f);

loop(44100)
{
    audioOut << audioIn * masterGain;
    advance();
}
```

This example will update the gain value every 44,100 samples

### Casting

Users are encouraged to initialize variables without any indication of bit size (i.e "int" or "float). However, there are certain variables and functions that can only be accessed via either 32 bit or 64 bit values. In this scenario, a user may recieve an error stating that 64 bit values must be explicitly cast to 32 bits. Explicit casting can be done as shown below:

```
int64 firstValue = 10;
int32 secondValue = int32(firstValue) * 2;
```


## The Components Window:

This is where you will add components that can be accessed from within your DSP Code. A component's value can be accessed from your DSP Code via the component's given name. The four types of components are:

1) **Slider** - This will output a stream of values, which is constantly updated to reflect the current position of the slider. This is a visual component.
```markdown
  Parameters:
  Variable Name: The slider's label and the variable name used to access the slider's current value 
  Min Value: The slider's minimum value  
  Max Value: The sliders maximum value  
  Initial Value: The slider's initial value  
  Step: The interval between slider values  
```
2) **Button** - This will output a binary value (0=OFF,1=ON). This is a visual component.
```markdown
  Parameters:  
  Variable Name: The button's label and the variable name used to access the button's current value  
```  
3) **Buffer** - This will store an array of values (most often an array of audio samples). Additionally, each Buffer created comes with a memory safe "indexer" variable that is initialized to zero. This indexer takes the name of the corresponding buffer, with "Index" appended.
```markdown
  Parameters:  
  Variable Name: The variable name used to access a buffer value  
  Data Type: The value type that the buffer stores (integer or floating point numbers)  
  Size: The number of values to be stored in the buffer  
``` 
4) **Number** - This will store a single numeric value.
```markdown
  Parameters:
  Name: The variable name used to access the variable's current value
  Initial Value: The variable's initial value  
  Data Type: The value type that the variable stores (integer or floating point number)  
```


## Need help?

Having trouble with EZDSP? Have suggested improvements. Please send all questions/comments to [garrettecklmusic@gmail.com](mailto:garrettecklmusic@gmail.com)

