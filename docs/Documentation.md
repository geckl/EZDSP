# EZDSP Documentation

The EZDSP interface consists of two main elements: The DSP Code Editor and the Components Window. Understanding how these elements interact is key to getting your first plug-in up and running.


## The DSP Code Editor:

This is where you will write your DSP code. This code exists within a loop, meaning that it will be executed iteratively for each incoming sample of audio.

EZDSP is powered by the SOUL programming language. More info on the SOUL language syntax can be found here: [SOUL Language Guide](https://github.com/soul-lang/SOUL/blob/master/docs/SOUL_Language.md)

### Data Types

SOUL supports int, float, complex, and string data types. Although all numeric data types can be initialized as 32 or 64 bit variables, EZDSP uses 32 bit ints and floats. To initialize a 32 bit float, an 'f' must be appended onto the end of the number. The best way to initialize numbers in EZDSP is as follows:

```
int i = 32;
float f = 32.0f;
float g = f + 1.5f;
```

#### Casting

Users are encouraged to initialize variables without any indication of bit size (i.e "int" or "float"). However, there are certain SOUL functions that require 64 bit values. In this scenario, a user may recieve an error stating that 64 bit values must be explicitly cast to 32 bits. Explicit casting can be done as shown below:

```
int64 sixtyFourBitValue = 10;
int thirtyTwoBitValue = int32(firstValue) * 2;
```

### SOUL Functions:

All SOUL functions and operators can be accessed from within the code editor. A full list of available functions can be found here: [SOUL Library Reference](https://soul-lang.github.io/SOUL/docs/soul_library.html)

While most SOUL functions require the use of the *soul::* namespace, a number of lower level functions can be called directly:

#### Arithmetic:

`abs()` `sqrt()` `pow()` `exp()` `log()` `log10()` `floor()` `ceil()` `fmod(numer, denom)` `remainder(numer, denom)`

#### Comparison and ranges:

`min(v1, v2)` `max(v1, v2)` `clamp(value, low, high)` `wrap(v, max)`

#### Trigonometry:

`sin()` `cos()` `tan()` `acos()` `asin()` `atan()` `atan2()` `sinh()` `cosh()` `tanh()` `asinh()` `acosh()` `atanh()`

### Global Variables:

These variables reflect important information about your current DAW Project:

`BPM` - The current tempo in beats per minute<br>
`CURRENTSAMPLE` - The current sample number starting from the beginning of the timeline<br>
`SAMPLERATE` - The current sample rate in hertz<br>
`SAMPLESPERBEAT` - The number of samples per quarter note, given the current tempo and sample rate<br>
`NUMERATOR` - The current time signature numerator (the number of beats per measure)<br>
`DENOMINATOR` - The current time signature denominator (the beat value)<br>

Additionally, the follow global constants are provided:

`pi` - Pi<br>
`twoPi` - Two PI<br>
`nan` - Not a Number<br>
`inf` - Infinity<br>
`processor.period` - the duration in seconds of one frame<br>
`processor.session` - a unique int32 which is different each time the program runs.<br>


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

