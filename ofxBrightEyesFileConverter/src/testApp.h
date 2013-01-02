/*
 
 Written by Ben McChesney 12/20/2012
 
 I wrote this simple OF wrapper for the Technology Will Save us Bright Eyes Kit
 
 it's totally awesome and you should get one.
 
 */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "LEDNode.h"

class testApp : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        //Movie setup
        void openMovie( ) ;
    
        //Conversion Methods
        void startConversion( ) ;
        void endConversion( ) ;
        void convertMovieFrame( ) ;
        void convertFboFrame( ) ;
    
        //Store all of our LED nodes
        vector<LEDNode> nodes ;
        
        //Movie Info
        ofVideoPlayer movie ;
        string videoPath ;
        bool bPlaying ;
        bool bConverting ;
        float movFrameRate ;
        int numFrames ;
    
        //LED Mapping information
        int mapWidth ;
        int mapHeight ;
        int numLeds ;
        int headerLen ;
        float dstW;
        float dstH;
        float srcW;
        float srcH;
    
        //The LEDs are awfully bright in a darkly lit environments
        float maxBrightness ;
    
        //Header string for the data file
        string header ;
        //Output array for conversion
        vector<unsigned char> outputArray ;
         
        //OfBuffer allows us to write to files
        ofBuffer buffer ;
    
        //We write either a movie or other pixel data to a Frame Buffer Object
        ofFbo fbo ;
    
        //Debug string of the current status
        string currentStatus ;
    
        //in and out for each movie
        float movieMin , movieMax ;

    
        
};
