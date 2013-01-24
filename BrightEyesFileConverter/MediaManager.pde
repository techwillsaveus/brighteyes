/**
 * Basic wrapper to handle various media types
 */
class MediaManager{
  
  private boolean available = false;
  private boolean finishedPlaying = false;
  private boolean isMovie = false;
  private boolean isImageSequence = false;
  private boolean isGif = false;
  private boolean isImage = false;
  private PImage currentFrame;
  private PImage img;
  private Movie movie;
  private ISPlayer imageSequence;
  private Gif gif;
  private int gifTotalFrames;
  private int gifCurrentFrame;
  
  private void reset(){
    available = true;
    finishedPlaying = false;
  }
  
  public boolean available(){
    return available;
  } 
  
  public void setImage(String path){
    currentFrame = loadImage(path);
    isMovie = isGif = isImageSequence = false;
    isImage = true;
    reset();
  }
  
  public void setImageSequence(PApplet parent,File f){
    if (imageSequence != null) imageSequence = null;
    imageSequence = new ISPlayer(parent, f);
    imageSequence.loop();
    isGif = isMovie = isImage = false; 
    isImageSequence = true;
    reset();
  }
  
  public void setMovie(PApplet parent,String path){
    movie = new Movie(parent, path);
    movie.loop();
    movie.play();
    movie.jump(0);
    movie.pause();
    movie.read();
    isMovie = true;
    isGif = isImageSequence = isImage = false; 
    reset();
  }
  
  public void setGif(PApplet parent,String path){
    gif = new Gif(parent, path);
    GifDecoder gifDecoder = new GifDecoder();
    gifDecoder.read(parent.openStream(path));
    gifCurrentFrame = gifTotalFrames = 0;
    for (int i = 0; i < gifDecoder.getFrameCount(); i++) 
      gifTotalFrames += gifDecoder.getDelay(i);
    gifTotalFrames *= 30;
    gifTotalFrames /= 1000;
    println(gifTotalFrames);
    println("getRepeat: " + gif.getRepeat());
    isMovie = isImageSequence = isImage = false;
    isGif = true;  
    reset();
  }
  
  public PImage getFrame(){
    PImage img = currentFrame;
    if (isMovie) img = movie;
    if (isGif)   img = gif;
    if (isImageSequence) img = imageSequence;
    return img;
  }
   
  public void update(){
    if(isMovie && movie != null && movie.available()) movie.read();
    if(isGif && gif.isPlaying()) {
      gifCurrentFrame++;
      if(gifCurrentFrame >= gifTotalFrames) {
        gifCurrentFrame = gif.getRepeat() == 0 ? 0 : gifTotalFrames;
        finishedPlaying = true;
      }
    }
    if(!isGif) finishedPlaying = (currentFrame() == totalFrames()-1);
  }
  
  public void play(){
    if (isMovie) { 
      movie.jump(0);
      movie.play();
    }
    if (isGif) { 
      gif.jump(0);
      gif.play();
    }
    if (isImageSequence) {
      imageSequence.jump(0);
      imageSequence.play();
    } 
  }
  public void pause(){
    if (isMovie) movie.pause();
    if (isGif) gif.pause();
    if (isImageSequence) imageSequence.pause();
  }
  
  public void stop(){
    if (isMovie) {
        movie.jump(0);
        movie.play();
        movie.stop();
      }
    if (isGif) {
      gif.jump(0);
      gif.stop();
      gifCurrentFrame = 0;
    }
    if (isImageSequence) {
      imageSequence.jump(0);
      imageSequence.stop();
    }
  }
  
  public int totalFrames(){
    int len = 0;
    if (isMovie && movie !=null) len = int(movie.duration() * movie.frameRate);
    if (isGif)   len = gif.getPImages().length;
    if (isImageSequence) len = imageSequence.totalFrames();
    if (isImage) len = 1;
    return len;
  }
  
  public int currentFrame(){
    int frame = 0;
    if (isMovie) frame = ceil(movie.time() * 30) - 1;
    if (isGif)   frame = gifCurrentFrame;//gif.currentFrame();
    if (isImageSequence) frame = imageSequence.currentFrame();
    if (isImage) frame = 1; 
    return frame;
  }
  
  public int progress(){
    int percent = 0;
    if (isMovie) percent = (int)(100.0*movie.time()/movie.duration());
    //if (isGif)   percent = (int)(100.0*gif.currentFrame()/gif.getPImages().length);
    if (isGif)   percent = (int)(100.0*gifCurrentFrame/gifTotalFrames);
    if (isImageSequence) percent = (int)(100.0*imageSequence.currentFrame()/imageSequence.totalFrames());
    return percent;
  }
   
  public boolean isImage(){   
    return isImage;
  }
  public boolean isImageSequence(){   
    return isImageSequence;
  }
  public boolean isGif(){   
    return isGif;
  }
  public boolean isMovie(){   
    return isMovie;
  }
  public boolean finishedPlaying(){
    return finishedPlaying;
  }
  
}
