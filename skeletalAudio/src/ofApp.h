#pragma once

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "ofxEyeTribe.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include <string>


#define NUM_CHANNELS 1
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024
#define PORT 1993

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
    
        //Individual update functions for code clarity
        void updateLeap();
        void updateEyeTribe();
    
        void drawLeap();
        void drawEyeTribe();
		
        void exit();
    
        void toggleRecording();
    
        //Eye Tribe Data
        ofxEyeTribe _tet;
    
        //Leap Motion Data
        ofxLeapMotion _leap;
	    vector <ofxLeapMotionSimpleHand> _simpleHands;
    
        vector <int> _fingersFound;
	    ofEasyCam _cam;
    
        //Saving XML data
        ofxXmlSettings _dataToRecord;
        std::string _dataFileName;
        unsigned long long _startMilliseconds;
    
        //OSC setup
        ofxOscReceiver _receiver;
        bool _isRecording;

};
