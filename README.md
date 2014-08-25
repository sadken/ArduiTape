ArduiTape
=========

Arduino WAV player to replace the tape drive on old 8-bit computers

ArduiTape.txt contains all the pin connections, and hopefully there will be some 
image files with the circuit diagrams for the low pass filter and main connections soon. 


Libraries:

ArduiTape needs both the SDfat and TMRpcm libraries with a couple of edits to the TMRpcm library. 
This is so we can use the SDfat library which made working through directories easier. 
Both the Sdfat and TMRpcm folders need to be placed into the libraries folder for the arduino IDE. 
I've included these here so you won't have to edit them yourself. 


Buttons:

Once everything is wired you should have 5 control buttons, Up, Down, Play/Pause, Stop, and motor control. 

Up/Down are disabled while a file is playing, and the play/pause button only works to start a file 
while motor control is turned on.


Amplifier:

The headphone amp used was taken from an old CD drive (the kind that had a volume control and headphone socket on it). 
The one i used had an LM4808 chip on it, and was quite easy to find solder points for the relevant pins. 
Although any 5v headphone amplifier could be used in its place. 


Wave Files:

In theory TMRpcm can handle WAV files up to 32,000 sample rate.  I've only tested this at 16,000 with my Acorn Electron
other machines should be fine with this.  

Use which ever TAP/UEF/etc to WAV converter program you can find, then open the file in an audio editor 
(sound recorder works fine in win XP) and convert it down to 16,000 samples.  
Amplifying the file to create a square wave (freeuef throws out a sinewave which the Electron doesn't like) 
helped the files load.  

Once the files are converted they can be placed onto the SD card, *note* the file list appears in the order the WAV
files are added so be aware of this. 

Directories are supported, but as SDfat has no easy way of moving through them i would only use one level of directory.
Each directory must have an extensionless file called "ROOT" placed in it (preferably before any other file to put it
at the top of the list).  When selected this will allow you to move back to the root directory of the SD card. 
