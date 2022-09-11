# EZDSP Example Patches

All example patches are included with the EZDSP plugin. These examples are the best way to understand how EZDSP works, and provide users with a basic starting point for a various types of digital effects.

## EZ Distortion

#### Complexity: Beginner

A simple distortion unit that applies a sigmoid function to the incoming signal

### Step 1: Add components

For this effect we'll need to create just one slider component:

SLIDER:  Variable Name = DRIVE, Min Value = -10, Max Value = 50, Initial Value = 0, Step = 1,<br>

### Step 2: Add digital signal processing

Now let's add some DSP code to turn these components into a delay unit:

```
let masterGain = pow (10.0f, GAIN * 0.05f); // [1]
let driveGain = pow (10.0f, DRIVE * 0.05f); // [2]
let driveSig = driveGain * audioIn; // [3]

float32 distSig;

if (driveSig > 0) // [4]
{
   distSig = 1 - pow(float32(2.718), -driveSig);
} else
{
   distSig = -1 + pow(float32(2.718), driveSig);
}

audioOut << distSig * masterGain / driveGain; // [5]

```

\[1\]: Convert the gain slider's decibel value into a gain factor multiplier<br>
\[2\]: Convert the drive slider's decibel value into a gain factor multiplier<br>
\[3\]: Multiple the current audio sample by the drive gain<br>
\[4\]: Check the polarity of the current audio sample so that it can be preserved. Apply the distortion formula: $f(x) = signalPolarity * (1-e^(-|x|))$ <br>
\[5\]: Output the distorted audio sample multiplied by the master gain divided by the drive gain.<br>

*We multiply by the drive gain at the beginning, and divide by the drive gain at the end, allowing the drive slider to increase/decrease the distortion level without controlling the overall gain (although applying heavy distortion will audibly decrease the volume).<br>


## EZ Delay

#### Complexity: Intermediate

A simple delay unit with controls for delay length and feedback

### Step 1: Add components

For this effect we'll need to create three components (two sliders and a buffer):

SLIDER:  Variable Name = DELAY, Min Value = 0, Max Value = 1, Initial Value = 0, Step = .01<br>
SLIDER:  Variable Name = FEEDBACK, Min Value = 0, Max Value = 100, Initial Value = 0, Step = 1 <br>
BUFFER:  Data Type = Float, Size = 44100, Name = DelayBuffer,<br>

### Step 2: Add digital signal processing

Now let's add some DSP code to turn these components into a delay unit:

```
let masterGain = pow (10.0f, GAIN * 0.05f); // [1]

let audioDelayed = (FEEDBACK/100) * DelayBuffer[DelayBufferIndex-int(DELAY*SAMPLERATE)];   // [2]

DelayBuffer[DelayBufferIndex] = audioIn + audioDelayed; // [3]

audioOut << (DelayBuffer[DelayBufferIndex-int(DELAY*SAMPLERATE)] + audioIn) * masterGain; // [4]

++DelayBufferIndex; // [5]

```

Here's what each line of code does:

\[1\]: Convert the gain slider's decibel value into a gain factor multiplier<br>
\[2\]: Multiply a delayed audio sample by a feedback scale factor and store it in a variable<br>
\[3\]: Add the current audio sample to the delayed audio sample and store in the current position of the delay buffer<br>
\[4\]: Output the current audio sample and a delayed audio sample from the delay buffer, multiplied by the master gain<br>
\[5\]: Increase the current position of the delay buffer by one sample<br>


## EZ Filter

#### Complexity: Advanced

A simple filter that can function as either a low-pass or high-pass

### Step 1: Add components

For this effect we'll need to create two components (one slider and one button):

SLIDER:  Variable Name = CUTOFF, Min Value = 100, Max Value = 20000, Initial Value = 1000, Step = 100<br>
BUTTON:  Variable Name = lowOrHi<br>

### Step 2: Add digital signal processing

Now let's add some DSP code to turn these components into a delay unit:

```
soul::filters::biquad::State[2] s;  //[1]
soul::filters::biquad::Coeffs[2] c; //[2]

loop    //[3]
{
    for (wrap<c.size> i)    //[4]
    {
        soul::filters::rbj_eq::update(c[i], SAMPLERATE, int(lowOrHi), CUTOFF, 0.5, 0.0);    //[5]
        c.at (i).setNormalised (c[i]); //[6]
    }

    let masterGain = pow (10.0f, GAIN * 0.05f); //[7]

    audioOut << soul::filters::biquad::processCascadeTDFII (audioIn, s, c) * masterGain;    //[8]
    advance();  //[9]
}

```

Here's what each line of code does:

\[1\]: Initialize a SOUL object for storing an array of biquad filter states<br>
\[2\]: Initialize a SOUL object that stores an array of biquad filter coefficients<br>
\[3\]: Create an infinite loop. Note that while all EZDSP code is executed in an outter loop by default, users can create their own inner loops to process samples in chunks. In this case, an inner loop is initialized to prevent the biquad state/coefficient variables from being reinitialized every sample<br>
\[4\]: Use a for-loop to iterate through the coefficient array<br>
\[5\]: Generate an updated set of filter's coefficients using SOUL's implementation of Robert Bristow-Johnson's biquad EQ, 2nd IIR filter.<br>

This function is defined as:

**void update (biquad::Coeffs& c, float64 sampleRate, int32 mode, float64 freqHz, float64 quality, float64 gaindB)** <br>

\[6\]: Apply the updated filter coefficients<br>
\[7\]:Convert the gain slider's decibel value into a gain factor multiplier<br>
\[8\]:Apply the filter to the current audio sample and output the result multiplied by the master gain<br>
