
#pragma once

#include <memory>
#include "ofVec3f.h"
#include "ofParameter.h"
#include "ofPolyline.h"

#include <thread>
#include <atomic>

#include "PathUtils.h"

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

template<class AgentType>
class ofxFlock {
public:
	
	ofxFlock();

	virtual void addAgent(ofVec3f pos=ofVec3f(0));
	
	virtual void update();
	
	const std::vector<std::shared_ptr<AgentType>>& getAgents() const { return mAgents; }

	AgentSettings& getSettings() { return mAgentSettings; }
	
protected:
	
	std::thread mCacheThread;
	std::atomic<bool> mCalcedCaches;
	
	void calcCaches();
	
protected:

	AgentSettings mAgentSettings;

	std::vector<ofVec3f> mPositions;
	std::vector<std::shared_ptr<AgentType>> mAgents;
	
	std::vector<ofVec3f> mCohesionCache, mSeparationCache;
	std::vector<int> mCohesionCacheCounts, mSeparationCacheCounts;
	
	size_t mNAgents;
		
	void wrapAgents();
};

struct FollowAgent : public Agent {
	std::shared_ptr<FollowPath> mPath;
	size_t mTargetIndex;
	
	FollowAgent(ofVec3f pos, AgentSettings &settings):
	Agent(pos, settings),
	mPath(nullptr),
	mTargetIndex(0) {}
	
	ofVec3f moveAlongPath() {
	
		const auto &vertices = mPath->getVertices();
		const auto &target = vertices[mTargetIndex];
		
		if (target.distance(mPos) < 3) {
//			mTargetIndex = (mTargetIndex + 1) % vertices.size();
		}
		
		return seekPosition(target);
	
	}
};

class ofxPathFollowingFlock : public ofxFlock<FollowAgent> {
public:
	
	ofxPathFollowingFlock();
	
	virtual void update() override;
	
	void addPathCollection(FollowPathCollection &&collection) {
		mPathCollections.push_back(std::move(collection));
	}
	
	void assignAgentsToCollection(int index=0);
	
	
	ofParameter<float> mFollowAmount;
	
protected:
	std::vector<FollowPathCollection> mPathCollections;
	
};