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
    currentFrame = 0 ;
    mapWidth = 0;
    mapHeight = 0;
    numLeds = 174;
    header = "TWSU-BRIGHT-EYES" ; 
    movieMin = 0.0f;
    movieMax = 1.0f ;

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
    
    currentStatus = "app initialized ! " ;
    
    fbo.allocate( INPUT_WIDTH , INPUT_HEIGHT, GL_RGBA ) ;
    fbo.begin() ;
        ofClear( 0 , 0 , 0 ) ;
    fbo.end( );
    videoPath = "default" ;
    maxBrightness = 255.0f ;
    timelineRect = ofRectangle( 75 , 5 , fbo.getWidth() , 35 ) ;
        
    ofBackground( 100 , 100 , 100 ) ;
    ofSetVerticalSync( true ) ;
    ofSetWindowTitle( "Bright Eyes LED Data Tool" ) ; 
}

//--------------------------------------------------------------
void testApp::update()
{
    //Convert the frame to update the LED display, even if not writing to file
    convertFboFrame() ;
    //If we are using a movie we stop when the movie is done playing
    if ( bPlaying == true && movie.getPosition() >= movieMax )
    {
        if ( bConverting == true )
            endConversion( ) ;
        else
        {
            movie.setPosition( movieMin ) ;
            movie.play( ) ; 
        }
    }
   
    movie.update( ) ;
}

//--------------------------------------------------------------
void testApp::draw()
{
        ofPushMatrix() ;
        //Give the render a little padding
        ofTranslate( 75 , 5 ) ;
    
        //Draw the movie timeline
        float barWidth = 10.0f ;
        ofPushStyle() ; 
            //Draw a rectangle at the between the min and max frames
            ofSetColor( 255 ) ;
            ofRect( 0 , 0 , timelineRect.width , timelineRect.height ) ;
            ofSetColor( 0 , 212 , 0 ) ;
            ofRect( movieMin * timelineRect.width - barWidth/2 , 0, barWidth, timelineRect.height ) ;
            ofRect( movieMax * timelineRect.width - barWidth/2 , 0, barWidth, timelineRect.height ) ;
            ofSetLineWidth( 4 ) ;
            ofLine(  movieMin * timelineRect.width , timelineRect.height / 2.0f ,  movieMax * timelineRect.width , timelineRect.height / 2 ) ;
        ofPopStyle() ;
        
        ofTranslate( 0 , timelineRect.height + 30 ) ;
        ofPushMatrix() ;
           ofDrawBitmapStringHighlight("source area:" , 4 , -8 ) ;
            fbo.begin() ;
            ofSetColor( 0 , 0 , 0 ) ;
            ofRect( 0 , 0 , ofGetWidth() , ofGetHeight() ) ;
            ofSetColor( 255 , 255 , 255 ) ;
            //If the movie is playing draw that to the FBO
            if ( bPlaying == true )
            {
                movie.draw( 0 , 0 ) ;
            }
            //Otherwise just draw something with OF
            else
            {
                //Opposite circles
                float height = fbo.getHeight() / 2.0f ;
                ofCircle( 140 , height  , sin ( ofGetElapsedTimef() ) * 130 ) ;
                ofCircle( 500 , height  , cos ( ofGetElapsedTimef() ) * 130 ) ;
                 
                /*
                //Split opening
                float h = fbo.getHeight() / 2 ;
                ofTranslate( 0 , h ) ;
                ofSetColor( 255 , 255 , 255 ) ;
                float curWidth = sin ( ofGetElapsedTimef() ) * ofGetWidth() ;
                ofSetRectMode( OF_RECTMODE_CENTER  ) ;
                ofRect( 0 , 0 , curWidth * 2 , fbo.getHeight() ) ;
                ofSetRectMode( OF_RECTMODE_CORNER  ) ;
                */
            }
            fbo.end() ;
            ofSetColor( 255 , 255 , 255 ) ;
            fbo.draw( 0 , 0 ) ;
        ofPopMatrix() ;
    
        ofEnableAlphaBlending() ;
        //Draw the LEDs
        ofTranslate( 0 , 240 ) ;
        ofDrawBitmapStringHighlight("LED Map:" , 4 , -2 ) ;
    
        ofSetColor( 255 , 255 , 255 ) ;
        for ( int i = 0 ; i <  nodes.size() ; i++ )
        {
            nodes[ i ].draw( ) ;
        }
    ofPopMatrix() ;
    
    //Create our instructions string
    string instructions = "C - is converting : " + ofToString( bConverting ) ;
    if ( bConverting == true )
    {
        int maxFrame = 300 ; 
        if ( bPlaying == true )
            maxFrame = movieMax * (float)movie.getTotalNumFrames() ;
        instructions += " converting frame : " + ofToString( currentFrame ) + " of " + ofToString( (int)maxFrame ) ;
    }
    
    instructions += "\nO - open file\nS - save file" ;
    instructions += "\n< / > MAX Brightness: " + ofToString( maxBrightness ) +"\n" + currentStatus ;
    
    ofDrawBitmapStringHighlight( instructions , ofPoint( 15 , ofGetHeight() - 65 ) ) ;

}

void testApp::convertFboFrame( )
{
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

    //Create an ofPixels object to copy the FBO data into
    ofPixels pix ;
    pix.allocate( fbo.getWidth() , fbo.getHeight() , GL_RGBA ) ;
    fbo.readToPixels( pix ) ;

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
                // extract the color from X / Y in the FBO
                dstY = m;
                srcY = (dstY/dstH)*srcH;
                ofColor col = pix.getColor( (int)srcX, (int)srcY ) ;
                float brightness = (col.r + col.g + col.b) / 3.0f ;
                //Apply a limit to the brightness value if so desired
                if ( brightness > maxBrightness )
                    brightness = maxBrightness ;
                totalBrightness += brightness ;
                nodes[ j ].brightness = brightness ;
                pixelCount++ ;
            }
        }
        if ( bConverting == true )
        {
            // write the average greyscale value across the rectangle to the output array
            outputArray.push_back( (unsigned char)( totalBrightness )/ pixelCount ) ;
        }
    }
    if ( bConverting == true ) currentFrame++ ;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    //cout << "keyPressed :: " << key << endl ;
    //Handle the key inputs
    switch ( key )
    {
            
        //lessthan
        case 44:
            maxBrightness--;
            if( maxBrightness < 0 ) maxBrightness = 0 ;
            break ;
            
        //greaterthan
        case 46 :
            maxBrightness++;
            if( maxBrightness > 255 ) maxBrightness = 255 ;
            break ;
            break ;
            
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
    movie.setPosition( movieMin ) ;
    movie.update() ;
    movie.play() ;
    currentFrame = movie.getCurrentFrame() ; 
    
    // place the header at the top of the output array
    for(int i=0; i< header.size() ; i++)
    {
        outputArray.push_back( (unsigned char)  header.at(i) ) ;
    }
    
    currentStatus = "starting conversion process! hit C again to end recording" ;
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
    ofFileDialogResult result = ofSystemLoadDialog( "pick a movie" , false ) ;
    
    if ( result.bSuccess == true )
    {
        //cout << "name :" << result.getName() << endl ;
        //cout << "file path: " << result.getPath() << endl ;
        movie.loadMovie( result.getPath() ) ;
        movie.setLoopState(OF_LOOP_NONE) ;
        float movieFPS = (float)movie.getTotalNumFrames() / movie.getDuration() ; 
        ofSetFrameRate( movieFPS ) ; 
        movie.setPosition( movieMin ) ; 
        movie.play() ;
        videoPath = result.getName() ;
        videoPath = videoPath.substr( 0 , videoPath.size() - 4 ) ;
        //cout << "videoPath : " << videoPath << endl ;
        currentStatus = "'" +videoPath + "'" + " loaded OK! Click on the timeline to set the in / outs for seemless looping" ;
        bPlaying = true ; 
    }
    else
    {
        currentStatus = " movie could NOT be LOADED :( " ;
    }
}

void testApp::checkTimelineInput( ofPoint mouse )
{
    //If a user clicks within the timeline area
    if ( bPlaying == true  && timelineRect.inside( mouse ) == true )
    {
        mouse.x -= timelineRect.x ;
        mouse.y -= timelineRect.y ;
        //Map the drag to be between 0 and 1
        float normalizedClick = ofMap ( mouse.x , 0 , timelineRect.width , 0.0f , 1.0f ) ;
        //check the distance and adjust the min / max video mapping
        float minDist = mouse.distance( ofVec2f( movieMin*timelineRect.width , timelineRect.height/2 ) ) ;
        float maxDist = mouse.distance( ofVec2f( movieMax*timelineRect.width , timelineRect.height/2 ) ) ;
        if ( minDist < maxDist )
            movieMin = normalizedClick ;
        else
            movieMax = normalizedClick ;
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
    checkTimelineInput( ofPoint ( x , y ) ) ;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    checkTimelineInput( ofPoint ( x , y ) ) ;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    checkTimelineInput( ofPoint ( x , y ) ) ;
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