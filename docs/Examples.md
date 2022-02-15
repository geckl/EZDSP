## EZ Delay

A simple delay unit with controls for delay length and feedback

### Step 1: Add components

For this effect we'll need to create three components (two sliders and a buffer):

SLIDER: Name = Delay, Min = 0, Max = 44100, Init = 0, Interval = 100
SLIDER: Name = Feedback, Min = 0, Max = 100, Init = 0, Interval = 1
BUFFER: Name = DelayBuffer, Type = Float, Size = 44100

Now let's add some DSP code to turn these components into a delay unit:

```
let audioDelayed = (Feedback/100) * DelayBuffer[DelayBufferIndex-int(Delay)];   // [1]

DelayBuffer[DelayBufferIndex] = audioIn + audioDelayed; // [2]

audioOut << DelayBuffer[DelayBufferIndex-int(Delay)] + audioIn; // [3]

++DelayBufferIndex; // [4]
```

Here's what each line of code does:

[1]: Multiply a delayed audio sample by a feedback scale factor and store it in a variable
[2]: Add the current audio sample to the delayed audio sample and store in the current position of the delay buffer
[3]: Output current audio sample and a delayed audio sample from the delay buffer
[4]: Increase the current position of the delay buffer by one sample

##EZ Distortion




##EZ Filter
