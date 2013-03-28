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
        ofRectangle area ; 
        int address ;
    
        unsigned char brightness ;  //how bright the LED is
        
    
        void setup ( int _address , int _leftX, int _rightX, int _topY, int _bottomY )
        {
            address = _address ;
            leftX = _leftX ;
            rightX = _rightX ;
            topY = _topY ;
            bottomY = _bottomY ; 
            brightness = ofRandom( 0 , 255 ) ;
            area = ofRectangle( _leftX , _topY , _rightX - _leftX , _bottomY - _topY ) ;
        }
    
        void draw( )
        {
            ofEnableSmoothing() ; 
            //Draw a simple rectangle with the brightness of the LED
            ofNoFill( ) ;
            ofSetColor( 255 ) ;
            ofSetLineWidth( 2 ) ;
            ofRect( area ) ;

            ofFill( ) ; 
            ofSetLineWidth( 1 ) ;
            ofSetColor( brightness , brightness , brightness , 255 ) ;
            ofRect( area ) ;
        }
    
    
};

#endif
