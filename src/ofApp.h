#pragma once

#include "ofMain.h"

#include "Path.h"
#include "Vehicle.h"
#include "ofxSvg.h"

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
    
    void randomisePoints();
    void sineWavePoints();
    void linePoints();
    void circlePoints();
    
    int circleMult;
    float circleThresh;
    bool doCirclePoints;
    bool doMove;
    float randAmount;
    whg::Path path, path2;
    vector<whg::Path> paths;
    vector<Vehicle*> points;
    
    ofxSVG svg;
    vector<ofPolyline> outlines;
    
    ofFbo fbo;
    ofImage thing;
    
    ofMutex mutex;


};

