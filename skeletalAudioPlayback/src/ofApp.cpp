#include "ofApp.h"
#include <sys/timeb.h>


#define screenWidth 2560
#define screenHeight 1080

#define topLeftX 420
#define topLeftY 30
#define topLeftZ -400
    
#define topRightX -90
#define topRightY -260
#define topRightZ -250
    
#define bottomRightX -200
#define bottomRightY -10
#define bottomRightZ -200
    
#define bottomLeftX 300
#define bottomLeftY 290
#define bottomLeftZ -360


#define CAMERA_EULER_X 137
#define CAMERA_EULER_Y 128
#define CAMERA_EULER_Z -6

#define CAMERA_X_START -616
#define CAMERA_Y_START -793
#define CAMERA_Z_START 613


//--------------------------------------------------------------
void ofApp::setup(){
    //Rendering Settings
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
    _cam.setOrientation(ofPoint(CAMERA_EULER_X, CAMERA_EULER_Y, CAMERA_EULER_Z));
    _cam.setPosition(ofPoint(CAMERA_X_START, CAMERA_Y_START, CAMERA_Z_START));
    
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    //Load the XML file to render from
    _dataFileName = "smokeyData.xml";
    loadDataFromFile();
    
    _currentFrameIdx = 0;
    
    
    //Load the sound file to play back
    _audioTrack.loadSound("smokey.aif");
    _audioTrack.play();
    
}

void ofApp::loadDataFromFile(){
  if(!_dataToRead.loadFile(_dataFileName)){
    printf("failed to load");
  }
  int numTimeStampedTags = _dataToRead.getNumTags("timeStampedData");
  for(int timeStampedNumber = 0; timeStampedNumber < numTimeStampedTags; timeStampedNumber++){
    frameData newFrame;
    _dataToRead.pushTag("timeStampedData", timeStampedNumber);
    newFrame.milliseconds = _dataToRead.getValue("milliseconds", -1);

    int numTags = _dataToRead.getNumTags("fingerData");
    for(int i = 0; i < numTags; i++) {
      _dataToRead.pushTag("fingerData", i);
      fingerData finger;
      finger.mcp = ofPoint(_dataToRead.getValue("mcp_x", 0),
                           _dataToRead.getValue("mcp_y", 0),
                           _dataToRead.getValue("mcp_z", 0));
        
      finger.tip = ofPoint(_dataToRead.getValue("tip_x", 0),
                           _dataToRead.getValue("tip_y", 0),
                           _dataToRead.getValue("tip_z", 0));
        
      finger.dip = ofPoint(_dataToRead.getValue("dip_x", 0),
                           _dataToRead.getValue("dip_y", 0),
                           _dataToRead.getValue("dip_z", 0));
        
      finger.pip = ofPoint(_dataToRead.getValue("pip_x", 0),
                           _dataToRead.getValue("pip_y", 0),
                           _dataToRead.getValue("pip_z", 0));
      
      newFrame.fingers.push_back(finger);
      _dataToRead.popTag();
    }
    
    //Eye data
    _dataToRead.pushTag("eyeData");
    ofPoint leftCenter = ofPoint(_dataToRead.getValue("leftEye_x",0),
                                 _dataToRead.getValue("leftEye_y", 0),
                                 0);
    ofPoint rightCenter = ofPoint(_dataToRead.getValue("rightEye_x",0),
                                 _dataToRead.getValue("rightEye_y", 0),
                                 0);
    ofPoint gazeCenter = ofPoint(_dataToRead.getValue("gaze_x",0),
                             _dataToRead.getValue("gaze_y", 0),
                             0);

    newFrame.leftEye.eyeCenter = leftCenter;
    newFrame.rightEye.eyeCenter = rightCenter;
    newFrame.gaze.eyeCenter = gazeCenter;
    
    _dataToRead.popTag();
    
    //Pop the timeStampedData
    _dataToRead.popTag();
    _allFrames.push_back(newFrame);
  }

}

//--------------------------------------------------------------
void ofApp::update(){
    if(_currentFrameIdx < _allFrames.size() - 1){
        //calculate the elapsed time since the recording started playing
        int audioSeconds = _audioTrack.getPositionMS();
        
        frameData nextFrame = _allFrames.at(_currentFrameIdx);
        _currentFrameIdx++;
        
        //GRAB THESE FROM DATA
        int frameMilliseconds = nextFrame.milliseconds;
        
        //compare the playing time with the current frame
        //if the frame is at a higher time stamp, do nothing and leave render data as is
        if(audioSeconds < frameMilliseconds){
          _currentFrameIdx--;
          return;
        }

        //if the frame is at a low time stamp, drop frames until at appropriate time stamp
        while (audioSeconds > nextFrame.milliseconds
               && _currentFrameIdx < _allFrames.size()){
          nextFrame = _allFrames.at(_currentFrameIdx);
          _currentFrameIdx++;
        }
        if(_currentFrameIdx < _allFrames.size()){
          _currentData =  _allFrames.at(_currentFrameIdx);
        }
    }else{
      _currentFrameIdx = 0;
      _audioTrack.setPosition(0);
      _audioTrack.play();
      //printf("SHUT ME DOWN!!!");
      //std::exit(0);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    _cam.begin();
    
	/**ofPushMatrix();
    ofRotate(90, 0, 0, 1);
    ofSetColor(20);
    ofDrawGridPlane(800, 20, false);
	ofPopMatrix();
    **/
    drawScreen();
    
    
    //draw the data currently stored in currentRenderData
    ofSetColor(0, 0, 0);

    for(std::vector<fingerData>::iterator it = _currentData.fingers.begin();
                                         it != _currentData.fingers.end(); it++){
        
        int ballJointSize = 6;
       //Draw ball joints
       fingerData currentF = *it;
       ofDrawSphere(currentF.mcp.x, currentF.mcp.y, currentF.mcp.z, ballJointSize);
       ofDrawSphere(currentF.pip.x, currentF.pip.y, currentF.pip.z, ballJointSize);
       ofDrawSphere(currentF.dip.x, currentF.dip.y, currentF.dip.z, ballJointSize);
       ofDrawSphere(currentF.tip.x, currentF.tip.y, currentF.tip.z, ballJointSize);
       
       //Draw finger lines
        ofSetLineWidth(10);
        ofLine(currentF.mcp.x, currentF.mcp.y, currentF.mcp.z, currentF.pip.x, currentF.pip.y, currentF.pip.z);
        ofLine(currentF.pip.x, currentF.pip.y, currentF.pip.z, currentF.dip.x, currentF.dip.y, currentF.dip.z);
        ofLine(currentF.dip.x, currentF.dip.y, currentF.dip.z, currentF.tip.x, currentF.tip.y, currentF.tip.z);
       
   }
   
   ofVec3f horizontalVector(topLeftX - topRightX, topLeftY - topRightY, topLeftZ - topRightZ);
   ofVec3f verticleVector(topLeftX - bottomLeftX, topLeftY - bottomLeftY, topLeftZ - bottomLeftZ);

   
   ofVec3f rightEyeHorizontalVector = horizontalVector * (_currentData.rightEye.eyeCenter.x/screenWidth);
   ofVec3f rightEyeVerticleVector = verticleVector * (_currentData.rightEye.eyeCenter.y/screenHeight);
   ofVec3f rightEyeVector = rightEyeHorizontalVector + rightEyeVerticleVector;
   
   ofVec3f leftEyeHorizontalVector = horizontalVector * (_currentData.leftEye.eyeCenter.x/screenWidth);
   ofVec3f leftEyeVerticleVector = verticleVector * (_currentData.leftEye.eyeCenter.y/screenHeight);
   ofVec3f leftEyeVector = leftEyeHorizontalVector + leftEyeVerticleVector;
   
   ofVec3f gazeHorizontalVector = horizontalVector * (_currentData.gaze.eyeCenter.x/screenWidth);
   ofVec3f gazeVerticleVector = verticleVector * (_currentData.gaze.eyeCenter.y/screenHeight);
   ofVec3f gazeVector = gazeHorizontalVector + gazeVerticleVector;
   

   int eye_z_offset = 300;
   
   ofSetColor(255, 255, 255);
   ofDrawSphere(topLeftX - rightEyeVector.x, topLeftY - rightEyeVector.y, topLeftZ - rightEyeVector.z + eye_z_offset, 10);
   ofDrawSphere(topLeftX - leftEyeVector.x, topLeftY - leftEyeVector.y, topLeftZ - leftEyeVector.z + eye_z_offset, 10);
   
   ofSetColor(0, 0, 0);
   ofDrawSphere(topLeftX - gazeVector.x, topLeftY - gazeVector.y, topLeftZ - gazeVector.z, 10);
   
   ofLine(topLeftX - rightEyeVector.x, topLeftY - rightEyeVector.y, topLeftZ - rightEyeVector.z + eye_z_offset,
         topLeftX - gazeVector.x, topLeftY - gazeVector.y, topLeftZ - gazeVector.z);
   ofLine(topLeftX - leftEyeVector.x, topLeftY - leftEyeVector.y, topLeftZ - leftEyeVector.z + eye_z_offset,
        topLeftX - gazeVector.x, topLeftY - gazeVector.y, topLeftZ - gazeVector.z);

   
   //ofVec3f pos =  _cam.getPosition();
   //printf("position: %f, %f, %f", pos.x, pos.y, pos.z);
   //ofVec3f orientation = _cam.getOrientationEuler();
   //printf("orientation: %f, %f, %f", orientation.x, orientation.y, orientation.z);
   
   _cam.end();
}


//--------------------------------------------------------------
void ofApp::drawScreen(){
    //Draw the screen location, counter clockwise from top left
    
    ofSetColor(0,0,0);
    //ofSetColor(150, 0, 0);
    ofDrawSphere(topLeftX, topLeftY, topLeftZ, 4);
    //ofSetColor(0, 150, 0);

    ofDrawSphere(topRightX, topRightY, topRightZ, 4);
    //ofSetColor(0, 0, 150);

    ofDrawSphere(bottomLeftX, bottomLeftY, bottomLeftZ, 4);
    ofDrawSphere(bottomRightX, bottomRightY, bottomRightZ, 4);
    
    ofLine(topLeftX, topLeftY, topLeftZ, topRightX, topRightY, topRightZ);
    ofLine(topRightX, topRightY, topRightZ, bottomRightX, bottomRightY, bottomRightZ);
    ofLine(bottomRightX, bottomRightY, bottomRightZ, bottomLeftX, bottomLeftY, bottomLeftZ);
    ofLine(bottomLeftX, bottomLeftY, bottomLeftZ, topLeftX, topLeftY, topLeftZ);


}


//--------------------------------------------------------------
void ofApp::exit(){

}