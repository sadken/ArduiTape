ArduiTape
=========

Arduino WAV player to replace the tape drive on old 8-bit computers

ArduiTape.txt contains all the pin connections, and hopefully there will be some 
image files with the circuit diagrams for the low pass filter and main connections soon. 

ArduiTape needs both the SDfat and TMRpcm libraries with a couple of edits to the TMRpcm library. 
This is so we can use the SDfat library which made working through directories easier. 
Both the Sdfat and TMRpcm folders need to be placed into the libraries folder for the arduino IDE. 
I've included these here so you won't have to edit them yourself. 

Once everything is wired you should have 5 control buttons, Up, Down, Play/Pause, Stop, and motor control. 

Up/Down are disabled while a file is playing, and the play/pause button only works to start a file 
while motor control is turned on.

The headphone amp used was taken from an old CD drive (the kind that had a volume control and headphone socket on it). 
The one i used had an LM4808 chip on it, and was quite easy to find solder points for the relevant pins. 
Although any 5v headphone amplifier could be used in its place. 

