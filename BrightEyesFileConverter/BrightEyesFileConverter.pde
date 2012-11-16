/**
 * Bright Eyes Demo file generator
 *
 * Converts an input from .mov (ideally ideally 627x220, 30 fps)
 * for use with Bright Eyes from TWSU (movieNNN.dat).
 *
 * Anything outside this size will be scaled, movies of differing
 * framerates will play at 30fps unless SDplayer.ino (firmware for
 * the glasses) is modified.
 *
 * (c) Stefan Dzisiewski-Smith 2012 for TWSU
 *
 */

// storage class for each LED node
private class node{
  public int leftX;
  public int rightX;
  public int topY;
  public int bottomY; 
 
  node(int leftX, int rightX, int topY, int bottomY){
    this.leftX = leftX;
    this.rightX = rightX;
    this.topY = topY;
    this.bottomY = bottomY;
  }
  
};

// movie
import processing.video.*;
Movie movie;
boolean playing = false;
boolean converting = false;
float movFrameRate = 30;
int numFrames = 0;

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
StringBuilder outputFileName = new StringBuilder("movie000.dat");
final int minMovieIndex = 0;
final int maxMovieIndex = 999;
final String header = "TWSU-BRIGHT-EYES";
int outputFileSize = 0;
byte[] outputArray;
int k;

// GUI
boolean showLeds = false;
boolean showText = true;

void setup() 
{
  // load the input movie and set it up
  movie = new Movie(this, dataPath("input.mov"));
  movie.loop();
  movie.play();
  movie.jump(0);
  movie.pause();
  
  // create output array based on number of frames and header
  numFrames = getMovLength();
  outputFileSize = headerLen + (numFrames*numLeds);
  outputArray = new byte[outputFileSize];

  // load mapping XML 
  XML inputXML = loadXML("bright-eyes-coords.xml");
  XML[] coordsXML = inputXML.getChildren("node");
  XML sizeXML = inputXML.getChild("outputSize");
  
  // extract map width and height
  mapWidth = sizeXML.getInt("width");
  mapHeight = sizeXML.getInt("height");
  
  // set app size to match map
  size(mapWidth, mapHeight);
  
  // check we have a correct map
  if(coordsXML.length != numLeds){
    print("Input XML length is ");
    print(coordsXML.length);
    print(" should be ");
    println(numLeds); 
    exit(); 
  } else {
  
    // if we do, create a node array
    nodes = new node[coordsXML.length];
    
    // populate the node array from the XML
    for (int i = 0; i < coordsXML.length; i++) {
      int address = coordsXML[i].getInt("address");
      int leftX = coordsXML[i].getInt("leftX");
      int rightX = coordsXML[i].getInt("rightX");
      int topY = coordsXML[i].getInt("topY");
      int bottomY = coordsXML[i].getInt("bottomY");
      nodes[address-1] = new node(leftX, rightX, topY, bottomY);
    }
  }
}

void draw() {
  // draw routine
  // draw the movie
  //image(movie, 0, 0, mapWidth, mapHeight);
  image(movie,0,0);
  
  // draw the LEDs, if appropriate
  if(showLeds){
    drawLeds();  
  }
  
  // create the text string, if appropriate
  if(showText){
    String status = "";
  
    if(converting){
      status = "Converting... " + (int)(100.0*movie.time()/movie.duration()) + "% complete"; 
    } else if(playing){
      status = "Playing... "  + (int)(100.0*movie.time()/movie.duration()) + "%";
    } else {
      status = "Press:\nc to convert\np to play / pause\nr to reset movie to the start\ns to overlay LEDs\nt to toggle text on / off";  
    } 
  
    // draw the text string
    noStroke();
    fill(255, 0, 0);
    text(status, 10, 20);
  }
  
}

void drawLeds(){
  
  // LED draw routine
  
  // set rectMode to match the way we store the LED co-ordinates
  rectMode(CORNERS);

  // set stroke and fill (yellow, 75/255 opacity)
  noStroke();
  fill(255, 255, 0, 75);

  // draw each rectangle, one by one
  for (int i = 0; i < nodes.length; i++) {
    rect(nodes[i].leftX, nodes[i].topY, nodes[i].rightX, nodes[i].bottomY);
  }    
}

void movieEvent(Movie m) {
  // Called every time a new frame is available to read
  
  // read the frame
  m.read();
  if(converting){
    // convert the frame if we need to
    convertFrame();
    
    // if we are at the end, finish conversion
    if(getMovFrame()==getMovLength()-1){
      endConversion();
    }   
  }
}

void keyPressed(){
  switch(key){
    case 'c':
      // convert the movie
      if(!converting){
        playing = false;
        beginConversion();
      }
      break;
    case 'r':
      // reset the movie to the beginning
      if(!converting){
        movie.jump(0);
        movie.play();
        movie.stop();
        playing = false;
        break;
      }
    case 's':
      // toggle showing the leds
      showLeds = !showLeds;
      break;
    case 'p':
      // play / pause
      if(!converting){
        playing = !playing;
        if(playing){
          movie.play();
        } else {
          movie.pause();  
        }
      }
      break;
    case 't':
      // toggle text visibility
      showText = !showText;
      break;
  }
}

void convertFrame(){
  
  // a few variables for the pixel mapping
  float dstX;
  float dstY;
  float srcX;
  float srcY;
  float accum;
  float pixelCount;
  
  // one LED at a time  
  for(int j=0; j<numLeds; j++){
    // reset the accumulator and pixel counter
    accum = 0.0;
    pixelCount = 0.0;
    // run through each pixel in the LED rectangle
    for(int l = nodes[j].leftX; l <= nodes[j].rightX; l++){
      // extract destination X and calculate source X in movie
      dstX = l;
      srcX = (dstX/dstW)*srcW;
      for(int m = nodes[j].topY; m <= nodes[j].bottomY; m++){
        // extract destination Y and calculate source Y in movie
        dstY = m;
        srcY = (dstY/dstH)*srcH;
        
        // add the R, G and B values of this pixel to our accumulator
        accum += red(movie.get((int)srcX, (int)srcY));
        accum += green(movie.get((int)srcX, (int)srcY));
        accum += blue(movie.get((int)srcX, (int)srcY));
        pixelCount += 3.0; // account for three pixels, one each of R, G, B
      }
    }
    
    // write the average greyscale value across the rectangle to the output array
    // and increment the array index (k)
    outputArray[k++] = (byte)(accum/pixelCount);
  }   
}
  


void beginConversion(){
  
  // begin conversion routine
  
  // initialise our output array index and mapping limits
  k = 0;
  dstW = mapWidth;
  dstH = mapHeight;
  srcW = movie.width;
  srcH = movie.height;
  
  // place the header at the top of the output array
  for(int i=0; i<headerLen; i++){
    outputArray[k++] = (byte)header.charAt(i);  
  } 
  
  // tell the user we are starting to convert
  println("begin conversion");
  
  // if we're not already at the beginning of the movie, go there, play and set the conversion flag
  movie.jump(0);
  movie.play();
  converting = true; 
}

void endConversion(){
  
  // end conversion routine
  
  // try to find a free file to output to
  for(int i=minMovieIndex; i<=maxMovieIndex; i++){
    
    // set the output file name to appropriately reflect the index
    outputFileName.setCharAt(5, (char)('0' + (i/100)));
    outputFileName.setCharAt(6, (char)('0' + ((i/10)%10)));
    outputFileName.setCharAt(7, (char)('0' + (i%10)));
    
    // generate a temporary file handle to see if it exists
    File outputFile = new File(dataPath(outputFileName.toString()));
    if(!outputFile.exists()){ 
      // if it doesn't exist, we have found a space
      
      // resize output array to account for inaccuracies in the frame counter
      // it always seems to overestimate, which is lucky... 
      // we will create a new array whose size reflects k, our counter thus far
      byte[] trimmedOutputArray = new byte[k];
      
      // then copy all the elements of the old array to the new
      // this is inefficient, but easy
      for(int j=0; j<k-1; j++){
        trimmedOutputArray[j] = outputArray[j]; 
      }
      
      // save the file, tell the user what name it has...
      saveBytes(dataPath(outputFileName.toString()), trimmedOutputArray);
      println("end conversion");
      println(outputFileName.toString());
      break;
    }
  }
  // ... and quit
  exit();
}

int getMovLength() {
  
  // tells us the length of the movie in frames (poorly, but seems to overestimate)
  
  return int(movie.duration() * movie.frameRate);
}  

int getMovFrame() { 

  // tells us which frame we are currently viewing / converting
  
  return ceil(movie.time() * 30) - 1;
}
