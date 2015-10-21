#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		
        void draw();
        void drawScreen();
    
        void loadDataFromFile();
    
        void exit();
    
        int _playbackTime;
        ofEasyCam _cam;

        //Sound Player
        ofSoundPlayer _audioTrack;
    
        //Reading XML data
        ofxXmlSettings _dataToRead;
        std::string _dataFileName;
    
        //structs for recording seen data
        typedef struct{
          ofPoint tip;
          ofPoint dip;
          ofPoint pip;
          ofPoint mcp;
        } fingerData;
    
        typedef struct{
          ofPoint eyeCenter;
        } eyeData;
    
        typedef struct{
          std::vector<fingerData> fingers;
          eyeData leftEye;
          eyeData rightEye;
          eyeData gaze;
          int milliseconds;
          
        } frameData;
    
        std::vector<frameData> _allFrames;
        frameData _currentData;
        int _currentFrameIdx;

};
