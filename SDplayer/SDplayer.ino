

// SD card file player for Bright-Eyes project

// (c) Stefan Dzisiewski-Smith, White Wing Logic and TWSU November 2012


/*  NOTE: 
 *
 *  The Bright Eyes bootloader uses EEPROM on the ATMEGA to 
 *  store the size and last-modified date of the firmware file 
 *  in order to avoid loading the same file at every boot. 
 *
 *  Using the same areas of EEPROM in your Arduino code will
 *  cause problems. The no-go areas of EEPROM are the last 8
 *  bytes (as we store two DWORDS) - these are EEPROM
 *  addresses 0x3F8..0x3FF inclusive. 
 *  
 *  If you wish to change this behaviour, you will need to
 *  edit fat16_readRootDirEntry() in mmc_fat_datesize.c (one
 *  of the bootloader source files)
 * 
 */


// SD card 
#include <SD.h>

// input movies
// manually added function prototype to overcome Arduino's autoprototyping
// this allows us to pass the index by reference and so update it
bool openMovie(int &index);
const int minMovieIndex = 0;
const int maxMovieIndex = 999;
int movieIndex = minMovieIndex;
File inputFile;
int fileSize;
const int headerSize = 16;
char fileName[13] = "movie000.dat"; // 8 in the filename, 1 period, 3 in the extension and 1 null terminator = 13

// timing
const int frameRate = 30; 
const int framePeriod = 1e6/frameRate; 
unsigned long lastMicros = 0;

// touch sense button
#include "touch.h"

// DMX
#include "dmx.h"
const int numLEDs = 174;
const int DMXframeSize = 180; // six extra bytes to keep Mike's drivers happy
const int LEDpin = 9;
unsigned char DMXframe[DMXframeSize]; 

void setup(){
  
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, HIGH); // turn LED off...
  
  // sets us up for DMX output
  setupDMX(DMXframe);
  
  // init SD card
  bool initOK = initSD();
 
   // if we have an SD init or movie error, sit in an infinite loop and don't 
   // continue program execution - flash the LED to indicate this error
   if(!initOK || !openMovie(movieIndex)){
     while(1){
       digitalWrite(LEDpin, LOW);
       delay(1000);
       digitalWrite(LEDpin, HIGH);
       delay(1000);
     };  
   }
}
 
void loop(){
  
  // loop function, controls frame timing
  
  if(micros()>=lastMicros+framePeriod){
    // if we're ready for a new frame, reset our timer and get the next frame
    lastMicros = micros();
    frameAdvance();
  }
  
}

void frameAdvance(){
  
  // advances the frame and sends it out to the LEDs
  
  if(readTouch()){
    digitalWrite(LEDpin, LOW); // pulse LED if we have a touch input
    if(++movieIndex>maxMovieIndex){
      movieIndex = minMovieIndex; 
    } 
    // open the next movie
    openMovie(movieIndex);
  } else {
    digitalWrite(LEDpin, HIGH); // switch LED off if we do not have a touch input
  }
  
  // whatever happens, get a new frame and send it out
  getNextFrame();
  sendDMX(DMXframe, DMXframeSize);  
}

void getNextFrame(){
  
  // loads the next frame from the SD card
  
  // if we don't have even one frame, we have nothing to do 
  if(inputFile.size()>=headerSize + numLEDs){  
    // only do this if we have enough bytes left to load for a full frame...
    if(inputFile.available()>=numLEDs){

      inputFile.read(DMXframe+1, numLEDs); // this should read everything in one quick block read
      
    } else {
      // ...else go back to the beginning of the file and start again
      inputFile.seek(headerSize); 
      getNextFrame ();   
    }
  }
}

boolean initSD(){
  
  // initialises the SD card and loads our input file
  
  String fileName;
  
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);// enable SD card power - THIS IS IMPORTANT!
  
  delay(100); // wait for SD card to stabilise...
  
  // pin10 is the CS pin for the BrightEyes board, if it is not
  // set as an output, the SD card code will not function
  pinMode(10, OUTPUT);

  if (!SD.begin(10)) {
    return false;
  }
}

bool openMovie(int &index){
  
  // opens a movie with a given index - for example, index 13 opens movie013.dat
  
  // if we're sent a silly index, return false
  if(index>maxMovieIndex || index<minMovieIndex){
    return false;  
  }
  
  // alter the filename to reflect the index
  fileName[5] = '0' + (index/100);
  fileName[6] = '0' + ((index/10)%10);
  fileName[7] = '0' + (index%10); 
  
  // check we have the file...
  if(SD.exists(fileName)){
    // ... if we do, close the old file and open the new one
    inputFile.close();
    inputFile = SD.open(fileName);
    // miss out the header
    inputFile.seek(headerSize);
    Serial.println(fileName);
    return true;
  } else {
    if(index==minMovieIndex){
      return false;  // if we can't find the first file, there is no hope
    } else {
      index = minMovieIndex; // otherwise, try again from the beginning (and update the index)
      return openMovie(index); 
    }
  }
}

