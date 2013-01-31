/**
 * Bright Eyes Editor
 * Preview and create binary .dat files for the Bright Eyes kit from TWSU
 * (c) George Profenza 2012 for TWSU
 */
import javax.swing.*;
import sojamo.drop.*;

final int headerLen = 16;
final int numLeds = 174;
ArrayList<int[]> frames = new ArrayList<int[]>();
int[] cframe,pframe = new int[numLeds];
String file,header = "TWSU-BRIGHT-EYES";
//brush padding
int px = 5;//original LED width = 5
int py = 9;//original LED height = 13
boolean usePreciseClick = false;
int brightness = 255;
//timeline  
int currentFrame = 0,totalFrames = 1,frameSkip=3;
boolean autoUpdate = false,showHelp = true;
int frameSize = 10,spacing = 2;
int mapX = 0,prevX = 0,mouseClickX = 0;
boolean[] keys = new boolean[526];
//status
int currentStatusFrames=0,totalStatusFrames = 0;
String status = "";

PShape glasses,leds; 
SDrop drop;

void setup(){
  textFont(createFont("Verdana",12));
  glasses = loadShape("brighteyes.svg");
  size((int)glasses.width,(int)glasses.height+(int)(glasses.height * .25));frameRate(30);
  leds = glasses.getChild("leds");
  frames.add(getFrame(0));
  drop = new SDrop(this);
}
void draw(){
  background(32);
  shape(glasses,0,0);
  pushStyle();
  if(autoUpdate && totalFrames > 1) currentFrame = ((currentFrame+1)%(totalFrames-1));
  cframe = frames.get(currentFrame);
  for(int i = 0 ; i < numLeds; i++){
    PShape led = leds.getChild(i);
    led.disableStyle();
    int index = int(led.getName().split("_")[1])-1;
    fill(cframe[index]);
    stroke(127);
    if(usePreciseClick ? led.contains(mouseX,mouseY) : isOver(led,mouseX,mouseY,px,py)){
      stroke(255);
      if(mousePressed) {
        if(keyPressed){
          if(keyCode == SHIFT) cframe[index] = 0;
          if(keyCode == ALT)   cframe[index] = (int)map(mouseY,led.getVertexY(0)-(usePreciseClick ? 0 : py),led.getVertexY(2)+(usePreciseClick ? 0 : py),0,255);
        }
        else cframe[index] = brightness;//cframe[index] = int(!boolean(cframe[index]/255)) * 255;//toggle values (scaled int to boolean, flip, then back to int and scaled back)
      }
    }
    shape(led,0,0);
  }
  popStyle();
  //timeline
  pushMatrix();
    float y = height/5*4.5;
    translate(mapX,y);
    pushStyle();
    noStroke();
    for(int i = 0 ; i < totalFrames; i++){
      float x = (frameSize+spacing)*i;
      if(((mouseX > x+mapX && mouseX < x+frameSize+mapX)&&(mouseY > y && mouseY < y+frameSize*2)) && mousePressed) currentFrame = i; 
      fill(i == currentFrame ? 192 : 255);
      rect(x,0,frameSize,frameSize*2);
    }
    popStyle();
  popMatrix();
  if(showHelp) drawHelp();
  //status
  pushStyle();
  if(currentStatusFrames > 0) currentStatusFrames--;
  noStroke();fill(map(currentStatusFrames,totalStatusFrames,0,255,32));
  text(status,5,height-3);
  popStyle();
  frame.setTitle((file != null ? file : "") + (" frame " + (currentFrame+1) + " of " + totalFrames));
}
boolean isOver(PShape s,int mx,int my,int px, int py){//!depends on shape vertices
  return ((mx > s.getVertexX(0)-px)&&(mx < (s.getVertexX(2))+px)&&
          (my > s.getVertexY(0)-py)&&(my < (s.getVertexY(2))+py));
}
int[] getFrame(int value){
  int[] result = new int[numLeds];
  Arrays.fill(result,value);
  return result;
}
void addFrame(){
  frames.add(getFrame(0));
  currentFrame++;
  totalFrames = frames.size();
}
void cloneFrame(){
  frames.add(cframe.clone());
  currentFrame++;
  totalFrames = frames.size();
}
void clearFrame(){
  Arrays.fill(cframe,0);
}
void removeFrame(){
  noLoop();
  frames.remove(currentFrame);
  currentFrame--;
  totalFrames = frames.size();
  loop();
}
void saveToDisk(String file){
  byte[] outputArray = new byte[headerLen+(numLeds * frames.size())];
  int i;
  for (i = 0; i < headerLen; i++) outputArray[i] = (byte)header.charAt(i);
  for(int[] frame : frames)//for each frame
    for(int b : frame)//for each byte in each frame
      outputArray[i++] = (byte)b;//copy the byte in the final output, offset of course by the header
  saveBytes(file, outputArray);
}
void clear(){
  noLoop();
  file=null;
  frames.clear();
  frames.add(getFrame(0));
  currentFrame = 0;
  totalFrames = frames.size();
  loop();
  setStatus("Horray! Brand New Animation !",1);
}
boolean checkKey(int k){
  if (keys.length >= k) return keys[k];  
  return false;
}
void keyPressed(){
  keys[keyCode] = true;
  if(keyCode == LEFT && currentFrame > 0)  currentFrame--;
  if(keyCode == RIGHT && currentFrame < totalFrames-1) currentFrame++;
  if(key == 'A' && px > 10) px-=10;//10x22
  if(key == 'D') px+=10;
  if(key == 'S' && py > 22) py-=22;
  if(key == 'W') py+=22;
  if(key == 'B' && brightness < 255) brightness+=5;
  if(key == 'b' && brightness > 0  ) brightness-=5;
  if(checkKey(ALT) && checkKey(KeyEvent.VK_C)) {
    arrayCopy(cframe,pframe);//copy frame
    setStatus("frame " + (currentFrame+1) + " copied", 1);
  }
  if(checkKey(ALT) && checkKey(KeyEvent.VK_V)) {
    arrayCopy(pframe,cframe);//paste frame
    setStatus("frame pasted", 1);
  }
  if(key == 'A' || key == 'S' || key == 'D' || key == 'W') setStatus("brush size changed",1);//fix brush size in LED unit
  if(key == 'b' || key == 'B') setStatus("new brush brightness : " + brightness, 1);
}
void keyReleased(){
  keys[keyCode] = false;
  if(key == ' ') autoUpdate = !autoUpdate;
  if(key == '=') addFrame();
  if(key == '+') cloneFrame();
  if(key == '-' && totalFrames > 1) removeFrame();
  if(key == 's') saveAnimation();
  if(key == 'n') clear();//new document/clear everything
  if(key == 'c') clearFrame();
  if(key == '?') showHelp = !showHelp;
}
void mousePressed() {
    prevX = mapX;
    mouseClickX = mouseX;
}
void mouseDragged() {
  if(keyPressed && keyCode == SHIFT && (mouseY > height/5*4.5)) {
    int twidth = (frameSize+spacing)*totalFrames;
    mapX = prevX + mouseX - mouseClickX;
    if(mapX < -twidth+width) mapX = -twidth+width;
    if(mapX > 0) mapX = 0;
  }
}
void dropEvent(DropEvent e){
  if(e.file().getName().endsWith("dat")){
    noLoop();
    file = e.file().getName();
    byte[] data = loadBytes(e.file().getAbsolutePath());
    if(data.length <= headerLen) {
      data = null;
      frame.setTitle(e.file().getName()+" is an empty file, try another");
    }else{
      frames.clear();//frames.remove(0);
      totalFrames = ((data.length-headerLen)/numLeds);
      currentFrame = 0;
      for(int f = 0; f < totalFrames; f++){
        int[] ef = new int[numLeds];
        for (int i = numLeds*f; i < (numLeds*f)+numLeds; i++) {
            int indexFrom = i+headerLen;
            int indexTo   = indexFrom-(numLeds*f+headerLen);
            ef[indexTo] = data[indexFrom];
        }
        frames.add(ef);      
      }
      showHelp = false;
      setStatus(file + " is now ready",1);  
    }
    loop();
  }
}
void drawHelp(){
  int pad = 10;
  pushStyle();
  noStroke();
  fill(0,200);
  rect(pad,pad,width-pad*2,height-pad*2);
  String help = "Welcome to the BrightEyes Editor\nPaint:\nClick and Drag to paint LEDs white/on\nSHIFT+Click and Drag to erase/paint LEDs black/off\n";
  help += "ALT+Click and Drag to control the fade/brightness of an LED.\nUse 'W'/'S' to increase/decrease brush size vertically and 'D'/'A' to increase/decrease the brush vertically.\n";
  help += "Use 'B' to increase the brush brigtness and 'b' to decrease it.\nFrame control:\nUse the LEFT/RIGHT cursor keys to navigate the timeline or Drag through the timeline (Shift+Drag to scroll it).\n";
  help += "SPACE toggles playback/ '=' adds a blank frame/ '+' copy current frame as last / '-' deletes the current frame/ \n'ALT+c' copies the current frame and 'ALT+v' pastes it/ 'c' clears the current frame\n";
  help += "File:\n'n' clears everything(new document)/ 's' saves the current document/ drag a .dat file to open an animation\n '?' toggles this help panel. Have fun!";
  fill(255);
  text(help,pad*2,pad*3);
  popStyle();
}
void setStatus(String message,float seconds){
  int frames = (int)(seconds * 30);
  currentStatusFrames = frames;
  totalStatusFrames = frames;
  status = message; 
}
void saveAnimation(){
  if(file == null) file = (String)JOptionPane.showInputDialog(frame, "name your creation", "Save BrightEyes animation", JOptionPane.PLAIN_MESSAGE)+".dat";
  saveToDisk(file);
  setStatus(file+" saved",1.5);
}
void saveAs(){
  file = null;
  saveAnimation();
}
