#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxFlock.h"
#include "ofxGui.h"
#include "ofxOsc.h"

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
//                mCurrentTime = message.getArgAsFloat(0);
                mCurrentTimeDiff = ofGetElapsedTimef() - message.getArgAsFloat(0);
                
//                auto now = ofGetElapsedTimef();
//                cout << now << " : " << mCurrentTime << " -> " << (now - mCurrentTime) << endl;
            }
            else {
                
                Message msg;
                msg.type = address;
                msg.time = message.getArgAsFloat(0);
                msg.from = message.getArgAsString(1);
                msg.to = message.getArgAsString(2);
                
                mMessages.push_back(std::move(msg));
                
//                cout << "received message for " << msg.time << endl;
            }
        }
    }
    
    deque<Message>& getMessages() { return mMessages; }
    const deque<Message>& getMessages() const { return mMessages; } // why?
    
    float getTimeDiff() const { return mCurrentTimeDiff; }
    
//    list<Message> getMessagesUntil(float time) {
//        
//        list<Message> output;
//        while (mMessages.back().time < time) {
//            output.push_back(mMessages.back());
//            mMessages.pop_back();
//        }
//        
//        return output;
//    }

    
protected:
    ofxOscReceiver mOscReceiver;
    
    deque<Message> mMessages;
    float mCurrentTimeDiff;
//    unordered_map<string, Message> mMessages;
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
    
    OscReceiver mOscReceiver;
    unordered_map<string, ofVec3f> mMacAddresses;
    unordered_map<string, shared_ptr<FollowPath>> mMacPaths;
    void createAddressPoints();
    
};



