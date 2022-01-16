## EZDSP Help

### What is EZDSP?

EZDSP is an audio plug-in that allows users to edit the plug-in's DSP source code from directly within their favorite DAW.  

### How does it work?


The EZDSP interface consists of two main elements: The DSP Code Editor and the Components Window. Understanding how these elements interact is key to getting your first plug-in up and running.

**The DSP Code Editor**

This is where you will write your DSP code. This code exists within a loop, meaning that it will be executed iteratively for each incoming sample of audio.

The DSP Code Editor is powered by the SOUL language, and as such all SOUL functions and operators can be accessed from within the "soul::" namespace. More info on the SOUL language syntax can be found here: [SOUL Documentation](https://github.com/soul-lang/SOUL/blob/master/docs/SOUL_Language.md)

While variables can be declared from within the DSP Code Editor, it's important to remember that this code is executed iteratively, meaning that any variables declared from the DSP Code Editor will be reset to their initial value for each sample processed. 

**The Components Window:**

This is where you will add global variables that can be accessed from within your DSP Code. These variables can be standalone values, or they can be linked to a slider or button. A component's value can be accessed from your DSP Code via the component's given name. The four types of components are:

1) Slider - This will output a stream of values, which is constantly updated to reflect the current position of the slider. This is a visual component.
```markdown
  Parameters:  
  Min: The slider's minimum value  
  Max: The sliders maximum value  
  Init: The slider's initial value  
  Step: The interval between slider values  
  Name: The slider's label and the variable name used to access the slider's current value  
```
2) Button - This will output a binary value (0=OFF,1=ON). This is a visual component.
```markdown
  Parameters:  
  Name: The button's label and the variable name used to access the button's current value  
```  
3) Buffer - This will store an array of values (most often an array of audio samples). Additionally, each Buffer created comes with a memory safe "indexer" variable that is initialized to zero. This indexer takes the name of the corresponding buffer, with "Index" appended.
```markdown
  Parameters:  
  Name: The variable name used to access a buffer value  
  Type: The value type that the buffer stores (integer or floating point numbers)  
  Size: The number of values to be stored in the buffer  
``` 
4) Global Variable - This will store a single value.
```markdown
  Parameters:  
  Init: The variable's initial value  
  Name: The variable name used to access the variable's current value  
  Type: The value type that the variable stores (integer or floating point numbers)  
```

### Keywords

Keyworks go here.

### Types

Types go here.

### Operators

Operators go here.

### SOUL Functions

Link to SOUL functions go here.

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```


### Need help?

Having trouble with EZDSP? Have suggested improvements. Please send all questions/comments to [garrettecklmusic@gmail.com](mailto:garrettecklmusic@gmail.com)

