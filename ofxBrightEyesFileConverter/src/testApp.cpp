#include "testApp.h"

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
    
    showText = true ;
    showLeds = true ;

    //Load out XML setup and positions
    ofxXmlSettings xml ;
    xml.loadFile( "bright-eyes-coords.xml" ) ;
    xml.pushTag("bright-eyes-coords") ;

    int numNodes = xml.getNumTags("node" ) ;
    for ( int i = 0 ; i < numNodes ; i++ )
    {
        //cout << "@ node " << i << endl ;
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
    
    currentStatus = " app initialized ! " ;
    
    fbo.allocate( 627 , 220, GL_RGB ) ;
    fbo.begin() ;
        ofClear( 0 , 0 , 0 ) ;
    fbo.end( );
    bRecordGenerative = false ;
}

//--------------------------------------------------------------
void testApp::update()
{
    movie.update( ) ;
    if ( bConverting == true )
    {
        if ( bRecordGenerative == false )
        {
            convertMovieFrame() ;
            if ( movie.getIsMovieDone() )
            {
                endConversion( ) ;
            }
        }
        else
        {
            convertFboFrame() ;
        }
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
    
    ofSetColor( 255 , 255 , 255 ) ;
    ofPushMatrix() ;
        //Draw the movie
        ofTranslate( 25 , 25 ) ;
        
        if ( bRecordGenerative == false )
        {
            movie.draw( 0 , movie.getHeight() ) ;
        }
        else
        {
            ofPushMatrix() ;
                ofTranslate( 0 , 220 ) ;
                fbo.begin() ;
                ofSetColor( 0 , 0 , 0 ) ;
                ofRect( 0 , 0 , ofGetWidth() , ofGetHeight() ) ;
                
                float h = fbo.getHeight() / 2 ; 
                ofTranslate( 0 , h ) ;
                ofSetColor( 255 , 255 , 255 ) ;
                float nowH = sin ( ofGetElapsedTimef() ) * h ; 
                ofRect( 0 , -nowH , fbo.getWidth() , nowH * 2 ) ;
                
                fbo.end() ;
                ofSetColor( 255 , 255 , 255 ) ;
                fbo.draw( 0 , 0 ) ;
            ofPopMatrix() ;
        }
        ofSetColor( 255 , 255 , 255 ) ;
        ofEnableAlphaBlending() ;
        //Draw the LEDs
        if ( showLeds == true )
        {
            ofTranslate( 0 , 0 ) ;
            for ( int i = 0 ; i <  nodes.size() ; i++ )
            {
                nodes[ i ].draw( ) ;
            }
        }
    ofPopMatrix() ;
    
    //Create our instructions string
    string instructions = "C - is converting : " + ofToString( bConverting) ;
    if ( bConverting == true )
    {
        instructions += " converting frame : " + ofToString( movie.getCurrentFrame() ) + " of " + ofToString( movie.getTotalNumFrames() )  + " to file : ";
    }
    
    instructions += "\nO - open file\nS - save file \nL - show LEDs : " + ofToString( showLeds) + " \nT - show text : " + ofToString( showText ) + "\nG - toggle generative " + ofToString( bRecordGenerative ) ; 
    
    instructions += "\n" + currentStatus ;
    
    ofDrawBitmapStringHighlight( instructions , ofPoint( 15 , ofGetHeight() - 115 ) ) ;

}

void testApp::convertMovieFrame( )
{
    // a few variables for the pixel mapping
    float dstX = 0.0f ;
    float dstY = 0.0f ;
    float srcX = 0 ;
    float srcY = 0 ;
    float accum = 0.0f ;
    float pixelCount = 0.0f ;
    
    srcW = movie.getWidth() ;
    srcH = movie.getHeight() ;
    dstW = movie.getWidth() ;
    dstH = movie.getHeight() ; 
    ofPixels pix = movie.getPixelsRef() ;
    
    // one LED at a time
    for(int j=0; j<numLeds; j++)
    {
        // reset the accumulator and pixel counter
        accum = 0.0;
        pixelCount = 0.0;
        
        int numPixels = 0 ;
        float totalBrightness = 0.0f ;
        
        //cout << " led @ " << j << endl ;
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
                // add the R, G and B values of this pixel to our accumulator
                accum += col.r ;
                accum += col.g ;
                accum += col.b ;
                pixelCount += 3.0; // account for three pixels, one each of R, G, B
            }
            
            numPixels++ ;
        }
        //cout << "totalBrightness : " << totalBrightness << " / numPixels " << numPixels << endl ;
        nodes[ j ].brightness = ( totalBrightness ) / (float) numPixels ;
        
        // write the average greyscale value across the rectangle to the output array
        outputArray.push_back( (unsigned char)(accum/pixelCount) ) ;
    }
}

void testApp::convertFboFrame( )
{
    // a few variables for the pixel mapping
    float dstX = 0.0f ;
    float dstY = 0.0f ;
    float srcX = 0 ;
    float srcY = 0 ;
    float accum = 0.0f ;
    float pixelCount = 0.0f ;
    
    srcW = movie.getWidth() ;
    srcH = movie.getHeight() ;
    dstW = movie.getWidth() ;
    dstH = movie.getHeight() ;
    
    ofPixels pix ;
    pix.allocate( fbo.getWidth() , fbo.getHeight() , 3 ) ;
    
    fbo.readToPixels( pix ) ; 
    srcW = fbo.getWidth() ;
    srcH = fbo.getHeight() ;
    dstW = fbo.getWidth() ;
    dstH = fbo.getHeight() ;
   
    // one LED at a time
    for(int j=0; j<numLeds; j++)
    {
        // reset the accumulator and pixel counter
        accum = 0.0;
        pixelCount = 0.0;
        
        int numPixels = 0 ;
        float totalBrightness = 0.0f ;
        
        //cout << " led @ " << j << endl ;
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
                // add the R, G and B values of this pixel to our accumulator
                accum += col.r ;
                accum += col.g ;
                accum += col.b ;
                pixelCount += 3.0; // account for three pixels, one each of R, G, B
            }
            
            numPixels++ ;
        }
        //cout << "totalBrightness : " << totalBrightness << " / numPixels " << numPixels << endl ;
        nodes[ j ].brightness = ( totalBrightness ) / (float) numPixels ;
        
        // write the average greyscale value across the rectangle to the output array
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
            if ( bConverting == true )
            {
                startConversion( ) ;
            }
            else
            {
                endConversion() ; 
            }
            break ;
            
        case 'g':
        case 'G':
            bRecordGenerative = !bRecordGenerative ;
            break ; 
            
    }
}

void testApp::startConversion( )
{
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
    //Everything has been written! Now let's output t a file
    //first we create the file
    string fileName = videoPath + ".dat" ;
    if ( bRecordGenerative == true )
        fileName = "generative.dat" ;
    
    ofFile dataFile = ofFile(  ) ;
     
    dataFile.create( ) ;
    dataFile.open( fileName , ofFile::ReadWrite , true ) ;
    buffer.clear() ;
    
    string bufferString = "" ;
    cout << "outputArray.size() : " << outputArray.size() ;
    for ( int i = 0 ; i < outputArray.size() ; i++ )
    {
        bufferString += outputArray[i] ;
    }
    
    cout << "Buffer string  " << endl << " <<< " << endl << bufferString << endl << " <<< " << endl ;
    //set our buffer from the string we just created
    buffer.set( bufferString ) ;
    
    dataFile.writeFromBuffer( buffer ) ;
    dataFile.create() ;
    dataFile.close() ;
    bool fileWritten = ofBufferToFile( fileName , buffer ) ;
    cout << "was the file written to : " << fileName << " : ? " << fileWritten << endl ;
    if ( fileWritten == true )
        currentStatus = " file was written OK! to : " + fileName ;
    else
        currentStatus = " file COULD NOT WRITE ! check the log : " + fileName ; 
    bConverting = false ; 
}

void testApp::loadMovie( )
{
    ofFileDialogResult result = ofSystemLoadDialog( "pick movie" , false ) ;
    
    if ( result.bSuccess == true )
    {
        cout << "name :" << result.getName() << endl ;
        cout << "file path: " << result.getPath() << endl ;
        movie.loadMovie( result.getPath() ) ;
        movie.setLoopState(OF_LOOP_NONE) ;
        movie.firstFrame() ;
        movie.play() ;
        numFrames = movie.getTotalNumFrames();
        int outputFileSize = headerLen + ( numFrames * numLeds ) ; 
        videoPath = result.getName() ;
        videoPath = videoPath.substr( 0 , videoPath.size() - 4 ) ;
        cout << "videoPath : " << videoPath << endl ;
        currentStatus = videoPath + " was loaded correctly! " ;
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