#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxFlock.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void exit();

    ofxSVG svg;
    
    ofFbo fbo;
    ofImage thing;
    
    ofMutex mutex;

	ofxPathFollowingFlock flock;

	ofxPanel gui;
	
	ofShader p2lShader;
};

