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
    
        void loadMovie( ) ;
        string videoPath ; 
    
        ofVideoPlayer movie ;
        bool bPlaying ;
        bool bConverting ;
        float movFrameRate ;
        int numFrames ;
    
        int mapWidth ;
        int mapHeight ;
        int numLeds ;
        int headerLen ;
        float dstW;
        float dstH;
        float srcW;
        float srcH;
    
        string header ;
        vector<unsigned char> outputArray ;
    
        // GUI
        bool showLeds ;
        bool showText ;
    
        vector<LEDNode> nodes ;
        void startConversion( ) ;
        void endConversion( ) ;
        void convertMovieFrame( ) ;
        void convertFboFrame( ) ; 
    
        ofBuffer buffer ;
        string currentStatus ;
    
        ofFbo fbo ;
        bool bRecordGenerative ;
};
