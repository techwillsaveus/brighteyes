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
    
        unsigned char brightness ; 
        
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
            ofPushMatrix( ) ;
                ofTranslate( rightX , topY ) ;
                
                ofSetColor( 0 , 0 , 0 ) ;
                ofSetLineWidth( 3 ) ;
                ofRect( 0 , 0 , rightX - leftX , topY - bottomY ) ; 

                ofSetLineWidth( 1 ) ; 
                ofSetColor( brightness ) ;
                ofRect( 0 , 0 , rightX - leftX , topY - bottomY ) ;
                //ofDrawBitmapStringHighlight( ofToString( address ) , ofPoint( -5 , -15 )) ;
            ofPopMatrix();
        }
    
    
};

#endif
