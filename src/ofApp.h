#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxFlock.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxRadioGroup.h"

class OscReceiver {
public:

    struct Message {
        string type;
        string from, to;
        float time;
    };

    OscReceiver() {
        mOscReceiver.setup(5557);
    }
    
    void update() {
        ofxOscMessage message;

        while (mOscReceiver.hasWaitingMessages()) {
            mOscReceiver.getNextMessage(message);
            auto address = message.getAddress();
            
            if (address == "/time") {
                mCurrentTimeDiff = ofGetElapsedTimef() - message.getArgAsFloat(0);
            }
            else {
                
                Message msg;
                msg.type = address;
                msg.time = message.getArgAsFloat(0);
                msg.from = message.getArgAsString(1);
                msg.to = message.getArgAsString(2);
                
                mMessages.push_back(std::move(msg));
                
            }
        }
    }
    
    deque<Message>& getMessages() { return mMessages; }
    const deque<Message>& getMessages() const { return mMessages; } // why?
    
    float getTimeDiff() const { return mCurrentTimeDiff; }


    
protected:
    ofxOscReceiver mOscReceiver;
    
    deque<Message> mMessages;
    float mCurrentTimeDiff;
};

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
    ofMesh mAgentMesh;


	ofxPanel gui;
    bool mDrawGui;
	
	ofShader p2lShader;
    
    ofParameter<bool> mCycleSettings;
    ofParameter<int> mAlpha;
    ofParameter<float> mImageSize;
    ofParameter<int> mPathIndex;
    void pathIndexChanged(int &index);
    
    ofTrueTypeFont mFont;
    
    ofParameter<bool> mAgentsArrived;
    size_t mArrivedCounter;
    
    ofParameter<float> mSecondsToWaitBeforeNext, mCloseDistanceThreshold;
    
    
    ofMatrix4x4 getTransformMatrix();
  
    void getDisplayMessage();
    
    
public:
    ofxPanel mSettingsPanel;
    ofxRadioGroup mSettingsGroup;
    vector<string> mSettingNames;
    
    void populateSettings();
    void settingChanged(ofxRadioGroupEventArgs &args);
};



