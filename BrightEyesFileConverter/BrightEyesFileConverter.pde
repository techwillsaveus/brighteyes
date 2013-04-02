/**
 * Bright Eyes Demo file generator
 *
 * Converts an input from .mov (ideally ideally 627x220, 30 fps)
 * for use with Bright Eyes from TWSU (inputFile.dat).
 *
 * Anything outside this size will be scaled, movies of differing
 * framerates will play at 30fps unless SDplayer.ino (firmware for
 * the glasses) is modified.
 *
 * (c) Stefan Dzisiewski-Smith 2012 for TWSU
 * Modified by George Profenza for TWSU
 */

import processing.video.*;
import gifAnimation.*;
import sojamo.drop.*;
import java.io.File;

MediaManager media;
boolean playing = false;
boolean converting = false;
boolean useAverage = true;
PImage currentFrame;
int gw = 36;//led columns (including blanks)
int gh = 7; //led rows    (including blanks)
int[] lut = {0  ,0  ,0  ,1  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,88 ,89 ,90 ,91 ,92 ,93 ,94 ,95 ,96 ,0  ,0  ,0  ,
             0  ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,19 ,20 ,21 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,97 ,98 ,99 ,100,101,102,103,104,105,106,107,108,0  ,
             22 ,23 ,24 ,25 ,26 ,27 ,28 ,29 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,0  ,0  ,0  ,0  ,0  ,0  ,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,
             37 ,38 ,39 ,40 ,41 ,42 ,43 ,44 ,45 ,46 ,47 ,48 ,49 ,50 ,51 ,0  ,0  ,0  ,0  ,0  ,0  ,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,
             52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,0  ,0  ,0  ,0  ,0  ,0  ,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,
             0  ,67 ,68 ,69 ,70 ,71 ,72 ,73 ,74 ,75 ,76 ,77 ,78 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,154,155,156,157,158,159,160,161,162,163,164,165,0  ,
             0  ,0  ,0  ,79 ,80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,166,167,168,169,170,171,172,173,174,0  ,0  ,0  };
ArrayList<byte[]> frames = new ArrayList<byte[]>(); //accumulates frames(byte arrays)
// mapping
node[] nodes;

int mapWidth = 0;
int mapHeight = 0;
final int numLeds = 174;
final int headerLen = 16;
float dstW;
float dstH;
float srcW;
float srcH;

// output file saving
String outputFileName;
final String header = "TWSU-BRIGHT-EYES";

// GUI
boolean showLeds = false;
boolean showText = true;
//drag'n'drop
SDrop drop;
//.dat preview
boolean previewMode = false;
boolean autoUpdate = true;
int previewFrame = 0;
int numPreviewFrames;
ArrayList<int[]> dframes = new ArrayList<int[]>();
//status
int currentStatusFrames=0,totalStatusFrames = 0;
String status = "";


void setup() 
{
  // create output array based on number of frames and header
  setupCoords();
  
  drop = new SDrop(this);
  media = new MediaManager();
}

void draw() {
  background(0);
  media.update();
  //conversion
  if (converting) {
    // convert the frame if we need to
    convertFrame();
    if(media.isImage()) endConversion();
    if(media.finishedPlaying()) endConversion();
  }
  // draw the movie
  if(media.available()) image(media.getFrame(),0,0);
  
  // draw the LEDs, if appropriate
  if (showLeds && !converting) {
    pushStyle();
    noStroke();fill(255,64);
    for(node n : nodes) rect(n.leftX,n.topY,n.rightX-n.leftX,n.bottomY-n.topY);
    popStyle();
  }
  //draw .dar preview
  if(previewMode){
    if(autoUpdate && numPreviewFrames > 1) previewFrame = ((previewFrame+1)%(numPreviewFrames-1));//update frames
    pushStyle();
    noStroke();
    fill(0);rect(0,0,width,height);
    for(int i = 0 ; i < numLeds; i++){
      node n = nodes[i];
      fill(dframes.get(previewFrame)[i]);
      rect(n.leftX,n.topY,n.rightX-n.leftX,n.bottomY-n.topY);
    }
    //for(node n : nodes) rect(n.leftX,n.topY,n.rightX-n.leftX,n.bottomY-n.topY);
    popStyle();
  }
  // create the text string, if appropriate
  if (showText) {
    String status = "";

    if (converting)   status = "Converting... " + media.progress() + "% complete";
    else if (playing) status = "Playing... " + media.progress() + "% complete";
    else status = "Drag a media file here to get started.\nPress:\nc to convert\np to play / pause\nr to reset movie to the start\ns to overlay LEDs\na to colour averaging on/off (currently: "+(useAverage ? "ON" : "OFF")+")\nt to toggle text on / off";
    
    // draw the text string
    noStroke();
    fill(255, 0, 0);
    text(status, 10, 20);
  }
  //status
  pushStyle();
  if(currentStatusFrames > 0) currentStatusFrames--;
  noStroke();fill(127,map(currentStatusFrames,totalStatusFrames,0,255,0));
  text(status,5,height-3);
  popStyle();
}

void keyPressed() {
  switch(key) {
  case 'a':
    useAverage = !useAverage;
    break;
  case 'c':
    // convert the movie
    if (!converting) {
      playing = false;
      beginConversion();
    }
    break;
  case 'r':
    // reset the movie to the beginning
    if (!converting) {
      media.stop();
      playing = false;
      break;
    }
  case 's':
    // toggle showing the leds
    showLeds = !showLeds;
    break;
  case 'p':
    // play / pause
    if (!converting) {
      playing = !playing;
      if (playing) media.play(); 
      else media.pause();
    }
    break;
  case 't':
    // toggle text visibility
    showText = !showText;
    break;
  }
}

void convertFrame() {
  if (useAverage) frames.add(getDMXFrameAvg(media.getFrame()));
  else{
    PImage smallFrame = media.getFrame().get();
    smallFrame.resize(gw,gh);
    frames.add(getDMXFrame(smallFrame));
  }
}

void beginConversion() {
  // begin conversion routine
  // initialise our output array index and mapping limits
  dstW = mapWidth;
  dstH = mapHeight;
  srcW = media.getFrame().width;
  srcH = media.getFrame().height;
  // tell the user we are starting to convert
  println("begin conversion ");

  // if we're not already at the beginning of the movie, go there, play and set the conversion flag
  media.stop();
  media.play();
  frames.clear();
  converting = true;
}

void endConversion() {
  saveToDisk(outputFileName);
  converting = false;
  setStatus("Conversion Complete! Previewing " + outputFileName,2.5);
  setupPreview(outputFileName);
}

byte[] getDMXFrame(PImage frame){
  byte[] result = new byte[numLeds];
  for(int i = 0 ; i < lut.length; i++)
    if(lut[i] > 0)
      result[lut[i]-1] = (byte)((int)brightness(frame.pixels[i]));//lut index is shifted by 1 because it needs to be a 0 based index
  return result;
}
byte[] getDMXFrameAvg(PImage frame){
  byte[] result = new byte[numLeds];
  float dstX,dstY,srcX,srcY;
  // one LED at a time  
  for (int j=0; j<numLeds; j++) {
    // reset the accumulator and pixel counter
    int clr = 0;
    // run through each pixel in the LED rectangle
    for (int l = nodes[j].leftX; l <= nodes[j].rightX; l++) {
      // extract destination X and calculate source X in movie
      dstX = l;
      srcX = (dstX/dstW)*srcW;
      for (int m = nodes[j].topY; m <= nodes[j].bottomY; m++) {
        // extract destination Y and calculate source Y in movie
        dstY = m;
        srcY = (dstY/dstH)*srcH;
        clr  = frame.get((int)srcX, (int)srcY);
      }
    }
    // write the average greyscale value across the rectangle to the output array
    result[j] = (byte)(brightness(clr));
  }
  return result;
}
void saveToDisk(String file){
  byte[] outputArray = new byte[headerLen+(numLeds * frames.size())];
  int i;
  for (i = 0; i < headerLen; i++) outputArray[i] = (byte)header.charAt(i);
  for(byte[] frame : frames)//for each frame
    for(byte b : frame)//for each byte in each frame
      outputArray[i++] = b;//copy the byte in the final output, offset of course by the header
  saveBytes(file, outputArray);
  println("bytes saved");
  converting = false;
}

void dropEvent(DropEvent e) {
  previewMode = false;
  showText = true;
  File f = e.file();
  String name = f.getName();
  outputFileName = name+".dat";
  if (f.isDirectory()) media.setImageSequence(this, f);
  else {
    if (name.toLowerCase().endsWith("gif")) media.setGif(this,f.getAbsolutePath());
    else {
      if (name.toLowerCase().endsWith("jpg")||
          name.toLowerCase().endsWith("png")||
          name.toLowerCase().endsWith("tga")||
          name.toLowerCase().endsWith("bmp")) media.setImage(f.getAbsolutePath());
      if (name.toLowerCase().endsWith("mov")||
          name.toLowerCase().endsWith("avi")||
          name.toLowerCase().endsWith("mpg")||
          name.toLowerCase().endsWith("mp4")||
          name.toLowerCase().endsWith("flv")||
          name.toLowerCase().endsWith("wmv")||
          name.toLowerCase().endsWith("3gp")||
          name.toLowerCase().endsWith("ogg")) media.setMovie(this, f.getAbsolutePath());
      if(name.toLowerCase().endsWith("dat")) setupPreview(e.file().getAbsolutePath());
    }
  }
}
void setupPreview(String path){
  playing = converting = false;
  previewMode = true;
  noLoop();
  byte[] data = loadBytes(path);
  if(data.length <= headerLen) {
    data = null;
    frame.setTitle(path+" is an empty file, try another");
  }else{
    dframes.clear();//frames.remove(0);
    numPreviewFrames = ((data.length-headerLen)/numLeds);
    previewFrame = 0;
    for(int fr = 0; fr < numPreviewFrames; fr++){
      int[] ef = new int[numLeds];
      for (int i = numLeds*fr; i < (numLeds*fr)+numLeds; i++) {
          int indexFrom = i+headerLen;
          int indexTo   = indexFrom-(numLeds*fr+headerLen);
          ef[indexTo] = data[indexFrom];
      }
      dframes.add(ef);      
    }
  }
  loop();
}
void setStatus(String message,float seconds){
  int frames = (int)(seconds * 30);
  currentStatusFrames = frames;
  totalStatusFrames = frames;
  status = message; 
}
