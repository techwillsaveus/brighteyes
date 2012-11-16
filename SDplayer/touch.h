#define touchThresh 2
#define touchHyst 1
#define touchPin (1<<7)
boolean touchFlag = false;
boolean lastTouch = false;

boolean readTouch(){
  
  char i = 0;
  boolean newTouch;
  
  // set touch pad as input and enable pullup
  DDRD &= ~touchPin; 
  PORTD |= touchPin; 
  
  // count up how long it takes to rise high
  for(i = 0; i < 16; i++){
    if(PIND & touchPin){
      break;
    }
  }
  
  // once we have our count value, set touch pad
  // as an output and drive it low, read for next time
  PORTD &= ~touchPin; 
  DDRD |= touchPin; 
  
  // implement hysteresis by having different thresholds depending
  // on whether we are currently registering a touch or not 
  // the falling threshold is lower than the rising one
  if(touchFlag){ 
    touchFlag = i > touchThresh;  
  } else {
    touchFlag = i > touchThresh + touchHyst;
  }
  
  // we have a new touch ONLY if we didn't have one last time and now do
  if(touchFlag && !lastTouch){
    newTouch = true;  
  } else {
    newTouch = false;  
  }
  
  // remember our last state for next time
  lastTouch=touchFlag;
  
  // return a value from our readTouch function
  return newTouch;
  
}
