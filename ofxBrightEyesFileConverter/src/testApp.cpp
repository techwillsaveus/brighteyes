#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    bPlaying = false ;
    bConverting = false ;
    movFrameRate = 30 ;
    numFrames = 0 ;
    
    mapWidth = 0;
    mapHeight = 0;
    numLeds = 174;
    headerLen = 16;
    
    ofSetFrameRate( 60 ) ;
    
    showText = true ;
    showLeds = true ;
    
    ofxXmlSettings xml ;
    xml.loadFile( "bright-eyes-coords.xml" ) ;
    xml.pushTag("bright-eyes-coords") ;

    int numNodes = xml.getNumTags("node" ) ;
    for ( int i = 0 ; i < numNodes ; i++ )
    {
        cout << "@ node " << i << endl ;
        LEDNode node ;
        int address = xml.getAttribute("node", "address", -1 , i ) ;
        cout << "address : " << address << endl ; 
        float leftX = xml.getAttribute( "node" , "leftX",  0.0f , i ) ;
        float rightX = xml.getAttribute(  "node" , "rightX" , 0.0f , i ) ;
        float topY = xml.getAttribute( "node" , "topY" , 0.0f , i ) ;
        float bottomY = xml.getAttribute( "node" , "bottomY" , 0.0f , i ) ;
        node.setup( address , leftX, rightX , topY , bottomY ) ;
        nodes.push_back( node ) ;
        
    }
    //<bright-eyes-coords>
    //<node address="1" leftX="63" rightX="68" topY="17" bottomY="29"></node>
}

//--------------------------------------------------------------
void testApp::update()
{
    movie.update( ) ;
    if ( bConverting == true )
    {
        convertFrame() ;
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetColor( 255 , 255 , 255 ) ;
    ofPushMatrix() ;
        ofTranslate( 25 , 25 ) ;
        movie.draw( 0 , 0 ) ;

        if ( showLeds == true )
        {
            ofTranslate( 0 , movie.getHeight() ) ; 
            for ( int i = 0 ; i <  nodes.size() ; i++ )
            {
                nodes[ i ].draw( ) ;
            }
        }
    ofPopMatrix() ;
    
    string instructions = "C - is converting : " + ofToString( bConverting) + "\nO - open file\nS - save file \nL - show LEDs : " + ofToString( showLeds) + " \nT - show text : " + ofToString( showText ) ;
    ofDrawBitmapStringHighlight( instructions , ofPoint( 15 , ofGetHeight() - 100 ) ) ;

}

void testApp::convertFrame( )
{
    // a few variables for the pixel mapping
    float dstX = 0.0f ;
    float dstY = 0.0f ;
    float srcX = 0 ;
    float srcY = 0 ;
    float accum = 0.0f ;
    float pixelCount = 0.0f ;
    
    ofPixels pix = movie.getPixelsRef() ;
    outputArray.clear() ; 
    // one LED at a time
    for(int j=0; j<numLeds; j++){
        // reset the accumulator and pixel counter
        accum = 0.0;
        pixelCount = 0.0;
        
        int numPixels = 0 ;
        float totalBrightness = 0.0f ;
        // run through each pixel in the LED rectangle
        for(int l = nodes[j].leftX; l <= nodes[j].rightX; l++)
        {
            // extract destination X and calculate source X in movie
            dstX = l;
            srcX = (dstX/dstW)*srcW;
            
            for(int m = nodes[j].topY; m <= nodes[j].bottomY; m++)
            {
                // extract destination Y and calculate source Y in movie
                dstY = m;
                srcY = (dstY/dstH)*srcH;
                
                ofColor col = pix.getColor( (int)srcX, (int)srcY ) ;
                float brightness = (col.r + col.g + col.b) / 3.0f ;
                cout << "brightness : " << brightness << endl ; 
  //              cout << "brightness : " << brightness << " srcX : " << srcX << " srcY " << srcY << endl ;
                totalBrightness += brightness ;
                nodes[ j ].brightness = brightness ;
                // add the R, G and B values of this pixel to our accumulator
                accum += col.r ;
                accum += col.g ; //green(movie.get((int)srcX, (int)srcY));
                accum += col.b ; //blue(movie.get((int)srcX, (int)srcY));
                pixelCount += 3.0; // account for three pixels, one each of R, G, B
            }
            
            numPixels++ ;
        }
        cout << "totalBrightness : " << totalBrightness << " / numPixels " << numPixels << endl ;
        nodes[ j ].brightness = ( totalBrightness ) / (float) numPixels ;
//        cout << "nodes [" << j << "] brightness " << nodes[j].brightness << endl ;
        
        // write the average greyscale value across the rectangle to the output array
        // and increment the array index (k)
        outputArray.push_back( (unsigned char)(accum/pixelCount) ) ;
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch ( key )
    {
        case 'o':
        case 'O':
            loadMovie( ) ; 
            break ;
            
        case 's':
        case 'S':
            saveOutputFile( ) ;
            break ;
            
        case 'l':
        case 'L':
            showLeds = !showLeds ; 
            break ;
            
        case 't':
        case 'T':
            showText = !showText ;
            break ;
            
        case 'c':
        case 'C':
            bConverting = !bConverting ;
///            startConversion( ) ;
            break ;
    }
}

void testApp::startConversion( )
{
    bConverting = true ; 
}

void testApp::loadMovie( )
{
    ofFileDialogResult result = ofSystemLoadDialog( "pick movie" , false ) ;
    
    if ( result.bSuccess == true )
    {
        cout << "name :" << result.getName() << endl ;
        cout << "file path: " << result.getPath() << endl ;
        movie.loadMovie( result.getPath() ) ;
        movie.play() ;
        
        numFrames = movie.getTotalNumFrames(); 
    }
    
}

void testApp::saveOutputFile( )
{
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}