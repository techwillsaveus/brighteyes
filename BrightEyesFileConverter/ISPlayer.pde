/**
 * Image Sequence Player
 * Heavily influenced by Patrick Meister's code
 */
public class ISPlayer extends PImage implements PConstants,Runnable {
    PApplet sketch;
    Thread runner;  
    private boolean play;
    private boolean loop;
    private int numFrames;
    private int currentFrame;
    private PImage[] frames;
    private int lastUpdate;
    private int delay = (int)(1000/30.0);//!rounded down
    private String[] formats = {"jpg","JPG","png","png","tga","TGA","bmp","BMP"};
    
    ISPlayer(PApplet sketch,File dir){
      super(1, 1, ARGB);
      this.sketch = sketch;
        if(!dir.exists()) throw new Error("The location is not valid:\n"+dir.getAbsolutePath()+"\nCheck if the path exists and is a folder/directory.");
        File[] files = dir.listFiles();
        ArrayList<String> paths = new ArrayList<String>();
        for(int i = 0 ; i < files.length; i++){
          String name = files[i].getName();
          for (int j = 0; j < formats.length; j++) {
          if(name.endsWith(formats[j])){
            paths.add(files[i].getAbsolutePath());
            j = formats.length;
          }
        }
        }
        numFrames = paths.size();
        frames = new PImage[numFrames];
        for(int i = 0 ; i < numFrames; i++) frames[i] = sketch.loadImage(paths.get(i));
        super.init(frames[0].width, frames[0].height, ARGB);
      runner = new Thread(this);
      runner.start();
      lastUpdate = sketch.millis();
    }
  
    public void run() {
      while (Thread.currentThread() == runner) {
        try {
          Thread.sleep(5);
        } catch (InterruptedException e) {
        }
        if (play) {
          if (sketch.millis() - lastUpdate >= delay) {
            if (currentFrame == numFrames - 1) {
              if (loop) jump(0);
              else     stop();
            }else jump(currentFrame + 1);
          }
        }
      }
    }
    public void dispose() {
      stop();
      runner = null;
    }
    public void jump(int where) {
      if (numFrames > where) {
        currentFrame = where;

        loadPixels();
        System.arraycopy(frames[currentFrame].pixels, 0, pixels, 0, width * height);
        updatePixels();
        
        lastUpdate = sketch.millis();
      }
    }
    public void stop() {
      play = false;
      currentFrame = 0;
    }
    public void play() {
      play = true;
    }
    public void loop() {
      play = true;
      loop = true;
    }
    public void noLoop() {
      loop = false;
    }
    public void pause() {
      play = false;
    }
    //getters
    public PImage[] getPImages() {
      return frames;
    }
    public boolean isPlaying() {
      return play;
    }
    public int currentFrame() {
      return currentFrame;
    }
    public int totalFrames(){
      return numFrames;
    }
    public boolean isLooping() {
      return loop;
    }
    
}
