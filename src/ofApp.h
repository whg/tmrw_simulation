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
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    int circleMult;
    float circleThresh;
    bool doCirclePoints;
    bool doMove;
    float randAmount;

    ofxSVG svg;
    vector<ofPolyline> outlines;
    
    ofFbo fbo;
    ofImage thing;
    
    ofMutex mutex;

	ofxPathFollowingFlock flock;

	ofxPanel gui;
	
	ofShader p2lShader;
};

