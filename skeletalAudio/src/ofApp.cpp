#include "ofApp.h"
#include <iostream>


//--------------------------------------------------------------
void ofApp::setup(){
    //Leap Motion Setup
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
	_leap.open();
    
	_cam.setOrientation(ofPoint(-20, 0, 0));
    
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    
    //Eye Tribe Setup
    ofSetCircleResolution(45);
    
    _tet.open();
    // tet.open(6555); //<---- if you want change device port
    
    
    //OSC setup
    _receiver.setup(PORT);
    _isRecording = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    updateLeap();
    updateEyeTribe();
    
    while(_receiver.hasWaitingMessages()){
      ofxOscMessage m;
      _receiver.getNextMessage(&m);
      toggleRecording();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
	_cam.begin();
    
    if(_isRecording){
        //Grab the system time
        time_t t = time(0);
        struct tm * now = localtime(&t);
        
        int elapsedMilliseconds = ofGetElapsedTimeMillis() - _startMilliseconds;
        
        int numTags = _dataToRecord.getNumTags("timeStampedData");
        _dataToRecord.addTag("timeStampedData");
        _dataToRecord.pushTag("timeStampedData", numTags);
        _dataToRecord.addValue("milliseconds", elapsedMilliseconds);
    }
    
    drawLeap();
    drawEyeTribe();
    
    if(_isRecording) {
      _dataToRecord.popTag();
    }
    _cam.end();
}


///////////////
////Update functions
///////////////


void ofApp::updateLeap(){
	_fingersFound.clear();
	
	//here is a simple example of getting the hands and drawing each finger and joint
	//the leap data is delivered in a threaded callback - so it can be easier to work with this copied hand data
	
	//if instead you want to get the data as it comes in then you can inherit ofxLeapMotion and implement the onFrame method.
	//there you can work with the frame data directly.
    
    
    
    //Option 1: Use the simple ofxLeapMotionSimpleHand - this gives you quick access to fingers and palms.
    
    
    _simpleHands = _leap.getSimpleHands();
    if( _leap.isFrameNew() && _simpleHands.size() ){
        
        _leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
		_leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        _leap.setMappingZ(-150, 150, -200, 200);
        
        fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
        
        for(int i = 0; i < _simpleHands.size(); i++){
            for (int f=0; f<5; f++) {
                int id = _simpleHands[i].fingers[ fingerTypes[f] ].id;
                ofPoint mcp = _simpleHands[i].fingers[ fingerTypes[f] ].mcp; // metacarpal
                ofPoint pip = _simpleHands[i].fingers[ fingerTypes[f] ].pip; // proximal
                ofPoint dip = _simpleHands[i].fingers[ fingerTypes[f] ].dip; // distal
                ofPoint tip = _simpleHands[i].fingers[ fingerTypes[f] ].tip; // fingertip
                _fingersFound.push_back(id);
            }
        }
    }
    
    //IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
	_leap.markFrameAsOld();
}


void ofApp::updateEyeTribe(){



}


///////////////
////Drawing functions
///////////////
    
void ofApp::drawLeap(){
    ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30),  OF_GRADIENT_BAR);
	
	ofSetColor(200);
	ofDrawBitmapString("ofxLeapMotion - Example App\nLeap Connected? " + ofToString(_leap.isConnected()), 20, 20);
    
	ofPushMatrix();
    ofRotate(90, 0, 0, 1);
    ofSetColor(20);
    ofDrawGridPlane(800, 20, false);
	ofPopMatrix();
    
    
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    for(int i = 0; i < _simpleHands.size(); i++){
        bool isLeft        = _simpleHands[i].isLeft;
        ofPoint handPos    = _simpleHands[i].handPos;
        ofPoint handNormal = _simpleHands[i].handNormal;
        
        ofSetColor(0, 0, 255);
        ofDrawSphere(handPos.x, handPos.y, handPos.z, 20);
        ofSetColor(255, 255, 0);
        ofDrawArrow(handPos, handPos + 100*handNormal);
        
        for (int f=0; f<5; f++) {
            ofPoint mcp = _simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
            ofPoint pip = _simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
            ofPoint dip = _simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
            ofPoint tip = _simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip
            
            ofSetColor(0, 255, 0);
            ofDrawSphere(mcp.x, mcp.y, mcp.z, 12);
            ofDrawSphere(pip.x, pip.y, pip.z, 12);
            ofDrawSphere(dip.x, dip.y, dip.z, 12);
            ofDrawSphere(tip.x, tip.y, tip.z, 12);
            
            if(_isRecording){
                //Record data to file
                int numTags = _dataToRecord.getNumTags("fingerData");
                _dataToRecord.addTag("fingerData");
                _dataToRecord.pushTag("fingerData", numTags);

                _dataToRecord.addValue("tip_x", tip.x);
                _dataToRecord.addValue("tip_y", tip.y);
                _dataToRecord.addValue("tip_z", tip.z);
                
                _dataToRecord.addValue("dip_x", dip.x);
                _dataToRecord.addValue("dip_y", dip.y);
                _dataToRecord.addValue("dip_z", dip.z);
                
                _dataToRecord.addValue("pip_x", pip.x);
                _dataToRecord.addValue("pip_y", pip.y);
                _dataToRecord.addValue("pip_z", pip.z);
                
                _dataToRecord.addValue("mcp_x", mcp.x);
                _dataToRecord.addValue("mcp_y", mcp.y);
                _dataToRecord.addValue("mcp_z", mcp.z);
                
                _dataToRecord.popTag();
            }
            
            ofSetColor(255, 0, 0);
            ofSetLineWidth(20);
            ofLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
            ofLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
            ofLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
        }
    }
}

void ofApp::drawEyeTribe(){

    // draw gaze data
    //-----------------------------------------------------------------------------
    if (_tet.isConnected())
    {
        ofFill();
        
        // red circle is raw gaze point
        ofSetColor(ofColor::red);
        ofCircle(_tet.getPoint2dRaw(), 5);
        
        // green dot is smoothed gaze point
        ofSetColor(ofColor::green);
        ofCircle(_tet.getPoint2dAvg(), 10);
        
        // when fixated is show orenge circle
        if (_tet.isFix())
        {
            ofSetColor(ofColor::orange, 100);
            ofCircle(_tet.getPoint2dAvg(), 40);
        }
        
        if(_isRecording){
            //Record data for later playback
            _dataToRecord.addTag("eyeData");
            _dataToRecord.pushTag("eyeData");
            
            _dataToRecord.addValue("leftEye_x", _tet.getLeftEyeRaw().x);
            _dataToRecord.addValue("leftEye_y", _tet.getLeftEyeRaw().y);
            
            _dataToRecord.addValue("rightEye_x", _tet.getRightEyeRaw().x);
            _dataToRecord.addValue("rightEye_y", _tet.getRightEyeRaw().y);
            
            _dataToRecord.addValue("gaze_x", _tet.getPoint2dAvg().x);
            _dataToRecord.addValue("gaze_y", _tet.getPoint2dAvg().y);

            _dataToRecord.popTag();
        }
        
        // and draw data from each eyes
        ofNoFill();
        ofSetColor(ofColor::aqua);
        ofCircle(_tet.getLeftEyeRaw(), 5);
        ofCircle(_tet.getRightEyeRaw(), 5);
        
        ofSetColor(ofColor::purple);
        ofCircle(_tet.getLeftEyeAvg(), 5);
        ofCircle(_tet.getRightEyeAvg(), 5);
        
        ofSetColor(ofColor::yellow);
        ofCircle(_tet.getLeftEyePcenter().x * ofGetWidth(), _tet.getLeftEyePcenter().y * ofGetHeight(), 20);
        ofCircle(_tet.getRightEyePcenter().x * ofGetWidth(), _tet.getRightEyePcenter().y * ofGetHeight(), 20);
    }

    
    
    // get gaze data and server state
    //-----------------------------------------------------------------------------
    gtl::ServerState s = _tet.getServerState();
    stringstream ss;
    ss << "fps: " << ofGetFrameRate() << endl;
    ss << endl;
    ss << "[ API ]" << endl;
    ss << "timestamp: " << _tet.getTimestamp() << endl;
    ss << endl;
    ss << "[ GAZE DATA ]" << endl;
    ss << "is fixed: " << (_tet.isFix() ? "TRUE" : "FALSE") << endl;
    ss << "point 2d raw: " << _tet.getPoint2dRaw() << endl;
    ss << "point 2d avg: " << _tet.getPoint2dAvg() << endl;
    ss << "left eye raw: " << _tet.getLeftEyeRaw() << endl;
    ss << "left eye avg: " << _tet.getLeftEyeAvg() << endl;
    ss << "left eye pupil size: " << _tet.getLeftEyePupilSize() << endl;
    ss << "left eye pupil coordinates normalized: " << _tet.getLeftEyePcenter() << endl;
    ss << "right eye raw: " << _tet.getRightEyeRaw() << endl;
    ss << "right eye avg: " << _tet.getRightEyeAvg() << endl;
    ss << "right eye pupil size: " << _tet.getRightEyePupilSize() << endl;
    ss << "right eye pupil coordinates normalized: " << _tet.getRightEyePcenter() << endl;
    ss << endl;
    ss << "[ SERVER STATE ]" << endl;
    ss << "framerate: " << s.framerate << endl;
    ss << "heartbeat interval: " << s.heartbeatinterval << endl;
    ss << "is calibrated: " << s.iscalibrated << endl;
    ss << "is calibrating: " << s.iscalibrating << endl;
    ss << "push: " << s.push << endl;
    ss << "tracker state: " << s.trackerstate << endl;
    ss << "version: " << s.version << endl;
    ss << endl;
    ss << "[ KEY ]" << endl;
    ss << "f: " << "toggle fulscreen" << endl;
    ss << "o: " << "open(connect)" << endl;
    ss << "c: " << "close(disconnect)" << endl;
    ss << "s: " << "starting server";
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(ss.str(), 20, 20);

}

void ofApp::toggleRecording(){
   _isRecording = !_isRecording;
   if(_isRecording){
        //Setup Output file
        time_t now = time(0);
        tm *ltm = localtime(&now);
        ostringstream os;
        os << "skeletalAudio"
           << ltm->tm_mday
           << ltm->tm_hour
           << ltm->tm_min
           << ltm->tm_sec
           << ".xml";
        _dataFileName = os.str();
        _dataToRecord.loadFile(_dataFileName);
       
        _startMilliseconds = ofGetElapsedTimeMillis();
       
       
   }else{
       _dataToRecord.saveFile(_dataFileName);
   }
}



//--------------------------------------------------------------
void ofApp::exit(){
    if(_isRecording){
      toggleRecording();
    }
    
    // let's close down Leap and kill the controller
    _leap.close();
}