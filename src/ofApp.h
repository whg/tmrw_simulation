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
    
    void keyPressed(int key);

    ofxSVG svg;
    
    ofFbo fbo;
    ofImage thing;
    
    ofMutex mutex;

	ofxPathFollowingFlock flock;

	ofxPanel gui;
	
	ofShader p2lShader;
        
    ofEasyCam cam;
    
    ofParameter<int> mAlpha;
    ofParameter<float> mImageSize;
    ofParameter<int> mPathIndex;
    void pathIndexChanged(int &index);
    
    ofTrueTypeFont mFont;
    
    ofParameter<float> mSphereSize;
    ofParameter<int> mSphereIterations;
};

