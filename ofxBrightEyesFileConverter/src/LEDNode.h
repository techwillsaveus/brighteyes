//
//  LEDNode.h
//  ofxBrightEyesFileConverter
//
//  Created by Ben McChesney on 12/21/12.
//
//

#ifndef ofxBrightEyesFileConverter_LEDNode_h
#define ofxBrightEyesFileConverter_LEDNode_h

class LEDNode
{   
    public :
        LEDNode( ) { }
        ~LEDNode( ) { }
    
        int leftX;                  
        int rightX;
        int topY;
        int bottomY;
        int address ;
    
        unsigned char brightness ;  //how bright the LED is
        
    
        void setup ( int _address , int _leftX, int _rightX, int _topY, int _bottomY )
        {
            address = _address ;
            leftX = _leftX;
            rightX = _rightX;
            topY = _topY;
            bottomY = _bottomY;
            brightness = ofRandom( 0 , 255 ) ; 
        }
    
        void draw( )
        {
            //Draw a simple rectangle with the brightness of the LED
            ofPushMatrix( ) ;
                ofTranslate( rightX , topY ) ;
                
                ofSetColor( 0 , 0 , 0 ) ;
                ofSetLineWidth( 3 ) ;
                ofRect( 0 , 0 , rightX - leftX , topY - bottomY ) ; 

                ofSetLineWidth( 1 ) ; 
                ofSetColor( brightness , brightness , brightness , 255 ) ;
                ofRect( 0 , 0 , rightX - leftX , topY - bottomY ) ;
            ofPopMatrix();
        }
    
    
};

#endif
