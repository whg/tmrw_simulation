
#pragma once

#include <memory>
#include "ofVec3f.h"
#include "ofParameter.h"
#include "ofPolyline.h"

#include <thread>
#include <atomic>

#include "PathUtils.h"

class ofxFlock {
public:
	
	ofxFlock();

	void addAgent(ofVec3f pos=ofVec3f(0));
	
	virtual void update();
	
	struct AgentSettings {
		ofParameter<float> maxSpeed, maxForce;
		ofParameter<float> cohesionDistance, separationDistance;
		ofParameter<float> cohesionAmount, separationAmount;
	};
	
	struct Agent {
		ofVec3f mPos;
		AgentSettings &mSettings;
		
		ofVec3f mVel, mAcc;
		
		Agent(ofVec3f pos, AgentSettings &settings):
		mPos(pos),
		mSettings(settings),
		mVel(0),
		mAcc(0) {}
		
		void update() {
			
			mVel+= mAcc;
			mAcc.set(0);
			
			mVel.limit(mSettings.maxSpeed);
			
			mPos+= mVel;
		}
		
		ofVec3f seek(ofVec3f direction) {
			return (direction - mVel).limit(mSettings.maxForce);
		}
		
		ofVec3f seekPosition(ofVec3f target) {
			return seek(target - mPos);
		}
		
		void apply(ofVec3f force) {
			mAcc+= force;
		}
		
	};
	
	
	const std::vector<std::shared_ptr<Agent>>& getAgents() const { return mAgents; }

	
	AgentSettings& getSettings() { return mAgentSettings; }
	
protected:
	
	std::thread mCacheThread;
	std::atomic<bool> mCalcedCaches;
	
	void calcCaches();
	
protected:

	AgentSettings mAgentSettings;

	std::vector<ofVec3f> mPositions;
	std::vector<std::shared_ptr<Agent>> mAgents;
	
	std::vector<ofVec3f> mCohesionCache, mSeparationCache;
	std::vector<int> mCohesionCacheCounts, mSeparationCacheCounts;
	
	size_t mNAgents;
		
	void cohesion();
	
	void wrapAgents();
};


class ofxPathFollowingFlock : public ofxFlock {
public:

	void addPathCollection(
	
	struct FollowAgent : public Agent {
		std::shared_ptr<FollowPath> mPath;
		
		FollowAgent(ofVec3f pos, AgentSettings &settings): Agent(pos, settings) {
			mPath = nullptr;
		}
	};
	
	std::vector<FollowPathCollection> mPathCollections;
	
//	void addAgent(const Path &path, ofVec3f pos);
	
};