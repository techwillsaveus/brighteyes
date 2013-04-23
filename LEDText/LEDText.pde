int[] lut = {0  ,0  ,0  ,1  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,88 ,89 ,90 ,91 ,92 ,93 ,94 ,95 ,96 ,0  ,0  ,0  ,
             0  ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,19 ,20 ,21 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,97 ,98 ,99 ,100,101,102,103,104,105,106,107,108,0  ,
             22 ,23 ,24 ,25 ,26 ,27 ,28 ,29 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,0  ,0  ,0  ,0  ,0  ,0  ,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,
             37 ,38 ,39 ,40 ,41 ,42 ,43 ,44 ,45 ,46 ,47 ,48 ,49 ,50 ,51 ,0  ,0  ,0  ,0  ,0  ,0  ,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,
             52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,0  ,0  ,0  ,0  ,0  ,0  ,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,
             0  ,67 ,68 ,69 ,70 ,71 ,72 ,73 ,74 ,75 ,76 ,77 ,78 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,154,155,156,157,158,159,160,161,162,163,164,165,0  ,
             0  ,0  ,0  ,79 ,80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,166,167,168,169,170,171,172,173,174,0  ,0  ,0  };
PImage mask;
int w = 36;//full width (including 'invisible' LEDs)
int h = 7; //full height
int s = 8; //scale factor, used for previewing on a computer
float x = w;//text x position
int   y = 6;//text y position 
int tw = 0; //text width
float speed = .8;//text scrolling speed
float start,end;
int brightness = 127;//text/led brightness

ArrayList<PFont> fonts = new ArrayList<PFont>();
int fontID = 0;

String message = "Hello ";//default text to display

boolean isPreviewing = false;
boolean isConverting = false;
boolean hasLooped = false;

ArrayList<byte[]> frames = new ArrayList<byte[]>(); //accumulates frames(byte arrays)
PGraphics buffer;//duplicate what we preview here, but keep it at the right size(36x7)
int numLeds = 174;
String header = "TWSU-BRIGHT-EYES";
int headerLen = 16;

void setup(){
  size(w*s,h*s,P2D);
  mask = loadImage("mask.png");
  String[] names = new java.io.File(dataPath(".")).list(); 
  for(int i = 0 ; i < names.length; i++) 
    if(names[i].toLowerCase().endsWith("vlw")) fonts.add(loadFont(names[i]));
  
  buffer = createGraphics(w,h);
  updateFont(fonts.get(fontID));
  updateBrightness(brightness);
  
  setText("123456789 123456789 123456789 123456789 123456789 123456789");
}
void updateFont(PFont font){
  textFont(font);
  buffer.beginDraw();
  buffer.textFont(font);
  buffer.endDraw();
}
void updateBrightness(int fill){
  fill(fill);
  buffer.beginDraw();
  buffer.fill(fill);
  buffer.endDraw(); 
}
void setText(String msg){
  message = msg;
  x = start;
  tw = (int)textWidth(message);
  hasLooped = false;
}
void convert(){
  isConverting = true;
  setText(message);
  frames.clear();
  println("starting conversion");
}
byte[] getDMXFrame(PImage frame){
  byte[] result = new byte[numLeds];
  for(int i = 0 ; i < lut.length; i++)
    if(lut[i] > 0)
      result[lut[i]-1] = (byte)((int)brightness(frame.pixels[i]));//lut index is shifted by 1 because it needs to be a 0 based index
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
  println(file+" ready!");
  isConverting = false;
}
void draw(){
  background(0);
  scale(s);
  boolean isRightToLeft = speed > 0;
  start = isRightToLeft ? w : -tw;
  end   = isRightToLeft ? -tw : w;
  if(isRightToLeft){
    if(x > end) x -= speed;
    else reset();
  }else{
    if(x < end) x -= speed;
    else reset();
  }
  if(mousePressed) y = mouseY;
  text(message,x,y);
  if(isConverting) {
    buffer.beginDraw();
    buffer.background(0);
    buffer.text(message,x,y);
    buffer.endDraw();
    frames.add(getDMXFrame(buffer));
  }
  if(isPreviewing) image(mask,0,0);
}
void reset(){
  hasLooped = true;
  x = start;
  if(isConverting) saveText();
}
void keyPressed(){
  if(keyCode == UP) speed += .1;
  if(keyCode == DOWN) speed -= .1;
  if(key == 'b' && brightness > 0) brightness--;
  if(key == 'B' && brightness < 255) brightness++;
  if(key == 'b' || key == 'B') updateBrightness(brightness);
  if(key == 'c') convert();
  if(key == 'r') x = start;  
  if(key == 'p') isPreviewing = !isPreviewing;
  if(key == 'f' && fontID > 0) fontID--;
  if(key == 'F' && fontID < fonts.size()-1) fontID++;
  if(key == 'f' || key == 'F'){  
    updateFont(fonts.get(fontID));
    setText(message);
  }
  if(key == 'T') setText(promptText());
}
String promptText(){
  return (String)javax.swing.JOptionPane.showInputDialog(frame, "text", "Add awesome text here", javax.swing.JOptionPane.PLAIN_MESSAGE);
}
void saveText(){
  println("conversion complete, name your file");
  String name = (String)javax.swing.JOptionPane.showInputDialog(frame, "name your creation", "Save BrightEyes text animation", javax.swing.JOptionPane.PLAIN_MESSAGE);
  if(name != null) saveToDisk(name+".dat");
}
