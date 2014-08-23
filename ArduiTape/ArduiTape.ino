// ---------------------------------------------------------------------------------
// DO NOT USE CLASS-10 CARDS on this project - they're too fast to operate using SPI
// ---------------------------------------------------------------------------------
// ArduiTape Arduino based 8-bit computer tape player. 
// Play WAV files from an SD card.  Based on the TMRpcm code.

#include <SdFat.h>
#include <TMRpcm.h> 
#include <LiquidCrystal.h>

LiquidCrystal lcd(14,15,5,4,3,2); //These can be moved about if needed

TMRpcm tmrpcm;   //Initialise tmrpcm

SdFat sd;    //Initialise Sd card 

SdFile entry;  //SD card file

char fileName[13];  //Current filename

const int chipSelect = 10;    //Sd card chip select pin

const int oldCard = SPI_HALF_SPEED;
const int newCard = SPI_QUARTER_SPEED;
const int fastCard = SPI_FULL_SPEED;

// ---------------------------------------------------------------------------------
// set cardType to 'oldCard' if using an old SD card (more than a few years old) or
// to 'newCard' if using a newly-purchase Class-4 card.
int cardType = fastCard;
// ---------------------------------------------------------------------------------

int btnPlay = 19;    //Play Button
int btnStop = 18;    //Stop Button
int btnUp = 17;      //Menu Up button
int btnDown = 16;    //Menu Down button
int btnMotor = 6;    //Motor Sense (connect pin to gnd to play, NC for pause)
int btnMselect = 7;  //Motor Control on/off button 

int mselectState = 1;//Motor control state 1=on 0=off
int wasPlaying = 0;  //Was there a file playing?
int motorState = 1;  //Current motor control state
int finished = 0;    //Has the file finished?
int start = 0;       //Currently playing flag
int pauseOn = 0;     //Pause state
int currentFile = 1; //Current position in directory
int maxFile = 0;     //Total number of files in directory
int isDir = 0;       //Is the current file a directory
unsigned long timeDiff = 0;  //button debounce
unsigned long timePress = 0; //button debounce

void setup() {
  lcd.begin(16,2);  //Initialise LCD (16x2 type)
  pinMode(chipSelect, OUTPUT); //Setup SD card chipselect pin
  if (!sd.begin(chipSelect,cardType)) {  //Start SD card and check it's working
    lcd_clearline(0);
    lcd.print("No SD Card");
    return;
  }
  sd.chdir();  //set SD to root directory

  tmrpcm.speakerPin = 9;  //Set speaker pin
  
  pinMode(btnPlay,INPUT_PULLUP);
  digitalWrite(btnPlay,HIGH);
  pinMode(btnStop,INPUT_PULLUP);
  digitalWrite(btnStop,HIGH);
  pinMode(btnUp,INPUT_PULLUP);
  digitalWrite(btnUp,HIGH);
  pinMode(btnDown,INPUT_PULLUP);
  digitalWrite(btnDown,HIGH);
  pinMode(btnMotor, INPUT_PULLUP);
  digitalWrite(btnMotor,HIGH);
  pinMode(btnMselect, INPUT_PULLUP);
  digitalWrite(btnMselect, HIGH);    //Setup buttons with internal pullup 
  getMaxFile();  //get the total number of files in the directory
  seekFile(currentFile);  //move to the first file in the directory
  lcd_clearline(0);
  lcd.print("Ready");
}

void loop(void) {
  if(!tmrpcm.isPlaying() && wasPlaying == 1) { 
    stopFile();
    //if the file has finished stop trying to play the file
  }
  motorState=digitalRead(btnMotor);
  if (millis() - timeDiff > 50) { // check switch every 100ms 
     timeDiff = millis(); // get current millisecond count
      
      if(digitalRead(btnPlay) == LOW) {
        if(start==0) {
          playFile();
          delay(200);
        } else {
        
          //timePress = millis();
          while(digitalRead(btnPlay)==LOW) {
            delay(50);
          }
          //if (millis() - timePress < 1000 && start == 1) {
            tmrpcm.pause();
            if (pauseOn == 0) {
              lcd_clearline(0);
              lcd.print("Paused");
              pauseOn = 1;
            } else {
              lcd_clearline(0);
              lcd.print("Playing");
              pauseOn = 0;
            }
          //if (millis() - timePress > 1000 && start == 1) {
          //   if (pauseOn == 1) {pauseOn = 0; tmrpcm.pause();}
          //   if (finished == 0) {
          //   } else {
          //   }
          // old code to test how long a button is held down. 
       }
     }
     if(digitalRead(btnMselect)==LOW){
       if(mselectState==0) {
         lcd_clearline(0);
         lcd.print("Motor CTRL On");
         mselectState=1; 
       } else { 
         lcd_clearline(0);
         lcd.print("Motor CTRL Off");
         mselectState=0; 
       }
       while(digitalRead(btnMselect)==LOW) {
         delay(50);
       }
     }
     if(digitalRead(btnStop)==LOW) {
       stopFile();
     }
     if(digitalRead(btnUp)==LOW && start==0) {
       upFile();
       while(digitalRead(btnUp)==LOW) {
         delay(50);  //wait until button is released
       }
     }
     if(digitalRead(btnDown)==LOW && start==0) {
       downFile();
       while(digitalRead(btnDown)==LOW) {
         delay(50);
       }
     }
     if(mselectState==1 && start==1) {  //if file is playing and motor control is on then handle current motor state
       if(motorState==1 && pauseOn==0) {
         tmrpcm.pause();
         lcd_clearline(0);
         lcd.print("Paused");
         pauseOn = 1;
       } 
       if(motorState==0 && pauseOn==1) {
         tmrpcm.pause();
         lcd_clearline(0);
         lcd.print("Playing");
         pauseOn = 0;
       }
     }
  }
}

void upFile() {    
  //move up a file in the directory
  
  currentFile--;
  if(currentFile<1) {
    getMaxFile();
    currentFile = maxFile;
  } 
  seekFile(currentFile);
}

void downFile() {    
  //move down a file in the directory
  
  currentFile++;
  if(currentFile>maxFile) { currentFile=1; }
  seekFile(currentFile);
}

void seekFile(int pos) {    
  //move to a set position in the directory, store the filename, and display the name on screen.
  
  entry.cwd()->rewind();
  for(int i=1;i<=currentFile;i++) {
    entry.openNext(entry.cwd(),O_READ);
    entry.getFilename(fileName);
    if(entry.isDir() || !strcmp(fileName, "ROOT")) { isDir=1; } else { isDir=0; }
    entry.close();
  }
  lcd_clearline(1);
  lcd.print(fileName);
}

void stopFile() {
  tmrpcm.stopPlayback();
  if(start==1){
    lcd_clearline(0);
    lcd.print("Stopped");
    start=0;
  }
}

void playFile() {
  if(isDir==1) {
    changeDir();
  } else {
    if(entry.cwd()->exists(fileName)) {
      lcd_clearline(0);
      lcd.print("Playing");
      lcd_clearline(1);
      lcd.print(fileName);
      tmrpcm.play(fileName);
      wasPlaying = 1;
      start=1; 
      tmrpcm.pause();
      lcd_clearline(0);
      lcd.print("Paused");
      pauseOn = 1;
    } else {
      lcd_clearline(1);
      lcd.print("No File Selected");
    }
  }
}

void getMaxFile() {    
  //gets the total files in the current directory and stores the number in maxFile
  
  entry.cwd()->rewind();
  maxFile=0;
  while(entry.openNext(entry.cwd(),O_READ)) {
    entry.getFilename(fileName);
    entry.close();
    maxFile++;
  }
  entry.cwd()->rewind();
}

void lcd_clearline(int l) {    
  //clear a single line on the LCD
  
  lcd.setCursor(0,l);
  lcd.print("                    ");
  lcd.setCursor(0,l);
}

void changeDir() {    
  //change directory, if fileName="ROOT" then return to the root directory
  //SDFat has no easy way to move up a directory, so returning to root is the easiest way. 
  //each directory (except the root) must have a file called ROOT (no extension)
                      
  if(!strcmp(fileName, "ROOT")) {
    sd.chdir(true);
  } else {
    sd.chdir(fileName, true);
  }
  getMaxFile();
  currentFile=1;
  seekFile(currentFile);
}
