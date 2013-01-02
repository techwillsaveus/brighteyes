#include "testApp.h"

//default video size
const int INPUT_WIDTH = 627 ;
const int INPUT_HEIGHT = 220 ;

//--------------------------------------------------------------
void testApp::setup()
{
    //setup and intialize all the values we need
    bPlaying = false ;
    bConverting = false ;
    movFrameRate = 30 ;
    numFrames = 0 ;
    
    mapWidth = 0;
    mapHeight = 0;
    numLeds = 174;
    headerLen = 16;
    header = "TWSU-BRIGHT-EYES" ; 
    ofSetFrameRate( 60 ) ;

    //Load out XML setup and positions
    ofxXmlSettings xml ;
    xml.loadFile( "bright-eyes-coords.xml" ) ;
    xml.pushTag("bright-eyes-coords") ;

    int numNodes = xml.getNumTags("node" ) ;
    for ( int i = 0 ; i < numNodes ; i++ )
    {
        //Iterate through each node XML tag and extract the values 
        int address = xml.getAttribute("node", "address", -1 , i ) ;
        //cout << "address : " << address << endl ;
        float leftX = xml.getAttribute( "node" , "leftX",  0.0f , i ) ;
        float rightX = xml.getAttribute(  "node" , "rightX" , 0.0f , i ) ;
        float topY = xml.getAttribute( "node" , "topY" , 0.0f , i ) ;
        float bottomY = xml.getAttribute( "node" , "bottomY" , 0.0f , i ) ;
        //Create and add our node
        LEDNode node ;
        node.setup( address , leftX, rightX , topY , bottomY ) ;
        nodes.push_back( node ) ;
    }
    
    currentStatus = " app initialized ! " ;
    
    fbo.allocate( INPUT_WIDTH , INPUT_HEIGHT, GL_RGB ) ;
    fbo.begin() ;
        ofClear( 0 , 0 , 0 ) ;
    fbo.end( );
    videoPath = "default" ;
    maxBrightness = 255.0f ; 
}

//--------------------------------------------------------------
void testApp::update()
{
    movie.update( ) ;
    
    convertFboFrame() ;
    //If we are using a movie we stop when the movie is done playing
    if ( bPlaying == true && movie.getIsMovieDone() == true )
    {
        endConversion( ) ;
    }
   
}

//--------------------------------------------------------------
void testApp::draw()
{
        ofPushMatrix() ;
        //Give the render a little padding
        ofTranslate( 75 , 25 ) ;
        ofPushMatrix() ;
           
            fbo.begin() ;
            ofSetColor( 0 , 0 , 0 ) ;
            ofRect( 0 , 0 , ofGetWidth() , ofGetHeight() ) ;
            
            ofSetColor( 255 , 255 , 255 ) ;
            //If the movie is playing record that to the FBO
            if ( bPlaying == true )
            {
                movie.draw( 0 , 0 ) ;
            }
            //Otherwise just draw something with OF
            else
            {
                /*
                //Opposite circles
                float offset = fbo.getWidth() / 3.0f ;
                float height = fbo.getHeight() / 2.0f ;
                ofCircle( offset , height  , sin ( ofGetElapsedTimef() ) * 125 ) ;
                ofCircle( offset * 2 , height  , cos ( ofGetElapsedTimef() ) * 125 ) ;
                 */
                
                //Eyelids opening
                float h = fbo.getHeight() / 2 ;
                ofTranslate( 0 , h ) ;
                ofSetColor( 255 , 255 , 255 ) ;
                float curWidth = sin ( ofGetElapsedTimef() ) * ofGetWidth() ;
                ofSetRectMode( OF_RECTMODE_CENTER  ) ;
                ofRect( 0 , 0 , curWidth * 2 , fbo.getHeight() ) ;
                ofSetRectMode( OF_RECTMODE_CORNER  ) ;
                
            }
            fbo.end() ;
            ofSetColor( 255 , 255 , 255 ) ;
            fbo.draw( 0 , 0 ) ;
        ofPopMatrix() ;
    
        ofSetColor( 255 , 255 , 255 ) ;
        ofEnableAlphaBlending() ;
        //Draw the LEDs
        ofTranslate( 0 , 240 ) ;
        for ( int i = 0 ; i <  nodes.size() ; i++ )
        {
            nodes[ i ].draw( ) ;
        }
    ofPopMatrix() ;
    
    //Create our instructions string
    string instructions = "C - is converting : " + ofToString( bConverting) ;
    if ( bConverting == true )
    {
        instructions += " converting frame : " + ofToString( movie.getCurrentFrame() ) + " of " + ofToString( movie.getTotalNumFrames() )  + " to file : ";
    }
    
    instructions += "\nO - open file\nS - save file" ;
    if ( bPlaying == true ) 
    instructions += "\n" + currentStatus ;
    
    ofDrawBitmapStringHighlight( instructions , ofPoint( 15 , ofGetHeight() - 45 ) ) ;

}

void testApp::convertFboFrame( )
{
    if ( bPlaying == false ) return ; 
    // a few variables for the pixel mapping
    float dstX = 0.0f ;
    float dstY = 0.0f ;
    float srcX = 0.0f ;
    float srcY = 0.0f ;
    float pixelCount = 0.0f ;
    
    //Source and Destination Width and Height
    srcW = fbo.getWidth() ;
    srcH = fbo.getHeight() ;
    dstW = fbo.getWidth() ;
    dstH = fbo.getHeight() ;
    
    ofPixels pix ;
    pix.allocate( fbo.getWidth() , fbo.getHeight() , GL_RGB ) ;
    
    pix = movie.getPixelsRef() ; 
    //fbo.readToPixels( pix ) ;
    
    
    // one LED at a time
    for(int j=0; j<numLeds; j++)
    {
        // reset the accumulator and pixel counter
        float totalBrightness = 0.0;
        pixelCount = 0.0;
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
                //cout << "brightness : " << brightness << " srcX : " << srcX << " srcY " << srcY << endl ;
                totalBrightness += brightness ;
                nodes[ j ].brightness = brightness ;
            }
        }
        if ( bConverting == true )
        {
            // write the average greyscale value across the rectangle to the output array
            outputArray.push_back( (unsigned char)( totalBrightness ) ) ;
        }
    }
    /*
           //cout << "totalBrightness : " << totalBrightness << " / numPixels " << numPixels << endl ;
        float _brightness = ( totalBrightness ) / (float) numPixels ;
        if ( _brightness > maxBrightness )
            _brightness = maxBrightness ;
        nodes[ j ].brightness = _brightness ; 
       */ 
    

}

/*
 ofColor col = pix.getColor( (int)srcX, (int)srcY ) ;
 float brightness = (col.r + col.g + col.b) / 3.0f ;
 //cout << "brightness : " << brightness << " srcX : " << srcX << " srcY " << srcY << endl ;
 totalBrightness += brightness ;
 nodes[ j ].brightness = brightness ;

 */


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    //cout << "keyPressed :: " << key << endl ;
    //Handle the key inputs
    switch ( key )
    {
        case 'o':
        case 'O':
            openMovie( ) ;
            break ;
            
        case 'c':
        case 'C':
            bConverting = !bConverting ;
            if ( bConverting == true )
            {
                startConversion( ) ;
            }
            else
            {
                endConversion() ; 
            }
            break ;
            
        default :
            break ;
            
    }
}

void testApp::startConversion( )
{
    //Set up all our values
    bConverting = true ;
    outputArray.clear() ;
    movie.firstFrame() ;
    movie.play() ;
    
    // place the header at the top of the output array
    for(int i=0; i<headerLen; i++)
    {
        outputArray.push_back( (unsigned char)  header.at(i) ) ;
    }
    
    currentStatus = " starting conversion process! " ;
}

void testApp::endConversion()
{
    //Everything has been written! Now let's output a file
    // first generate a file name
    string fileName = videoPath + ".dat" ;
    
    //create the ofFile
    ofFile dataFile = ofFile(  ) ;
    dataFile.create( ) ;
    
    //attemtpt to open the file, this will override a previous recording
    dataFile.open( fileName , ofFile::ReadWrite , true ) ;
    
    //clear the buffer just to be sure there's nothing leftover
    buffer.clear() ;
    
    //append each byte to a single string
    string bufferString = "" ;
    for ( int i = 0 ; i < outputArray.size() ; i++ )
    {
        bufferString += outputArray[i] ;
    }
    
    //cout << "Buffer string  " << endl << " <<< " << endl << bufferString << endl << " <<< " << endl ;
    //set our buffer from the string we just created
    buffer.set( bufferString ) ;
    
    //write to the buffer !
    dataFile.writeFromBuffer( buffer ) ;
    dataFile.create() ;
    dataFile.close() ;
    bool fileWritten = ofBufferToFile( fileName , buffer ) ;
    //Check if the file was written correctly
    cout << "was the file written to : " << fileName << " : ? " << fileWritten << endl ;
    if ( fileWritten == true )
        currentStatus = " file was written OK! to : " + fileName ;
    else
        currentStatus = " file COULD NOT WRITE ! check the log : " + fileName ; 
    bConverting = false ; 
}

void testApp::openMovie( )
{
    ofFileDialogResult result = ofSystemLoadDialog( "pick movie" , false ) ;
    
    if ( result.bSuccess == true )
    {
        cout << "name :" << result.getName() << endl ;
        cout << "file path: " << result.getPath() << endl ;
        movie.loadMovie( result.getPath() ) ;
        movie.setLoopState(OF_LOOP_NONE) ;
        movieMin = 0.0f;
        movieMax = 1.0f ; 
        movie.firstFrame() ;
        movie.play() ;
        numFrames = movie.getTotalNumFrames();
        int outputFileSize = headerLen + ( numFrames * numLeds ) ; 
        videoPath = result.getName() ;
        videoPath = videoPath.substr( 0 , videoPath.size() - 4 ) ;
        cout << "videoPath : " << videoPath << endl ;
        currentStatus = videoPath + " was loaded correctly! " ;
        bPlaying = true ; 
    }
    else
    {
        currentStatus = " movie could NOT be LOADED " ;
    }
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