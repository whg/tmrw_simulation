
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
	
	ofParameter<bool> mDoFlock;
	
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

inline ofVec3f getNormalPoint(ofVec3f p, ofVec3f a, ofVec3f b) {
	ofVec3f ap = p - a;
	ofVec3f ab = b - a;
	ab.normalize();
	ab *= ap.dot(ab);
	return a + ab;
}

struct FollowAgent : public Agent {
	std::shared_ptr<FollowPath> mPath;
	size_t mTargetIndex;
	
	FollowAgent(ofVec3f pos, AgentSettings &settings):
	Agent(pos, settings),
	mPath(nullptr),
	mTargetIndex(0) {}
	
	ofVec3f moveToNextTarget() {
	
		const auto &pathVertices = mPath->getVertices();
		const auto &target = pathVertices[mTargetIndex];
		
		if (target.distance(mPos) < 3) {
			mTargetIndex = (mTargetIndex + 1) % pathVertices.size();
		}
		
		return seekPosition(target);
	
	}
	
	ofVec3f moveAlongPath() {
		
		auto nextPosition = mPos + mVel * 5;
		const auto &pathVerts = mPath->getVertices();
		const auto nVerts = pathVerts.size();
		
		float minDist = 1e8, dist;
		ofVec3f a, b, normalPoint, dir, target;
		
		for (size_t i = 0; i < nVerts; i++) {
			
			a = pathVerts[i];
			b = pathVerts[(i+1) % nVerts];
	
			normalPoint = getNormalPoint(nextPosition, a, b);
			
			if (normalPoint.x < min(a.x, b.x) ||
				normalPoint.x > max(a.x, b.x) ||
				normalPoint.y < min(a.y, b.y) ||
				normalPoint.y > max(a.y, b.y)) {
				
				normalPoint = a;
				
				a = b;
				b = pathVerts[(i+2) % nVerts];
				
			}
			
			dir = b - a;
			
			dist = nextPosition.squareDistance(normalPoint);
			
			if (dist < minDist) {
				
				minDist = dist;
				
				target = normalPoint + dir * 10;
			}
		}
		
		// if minDist > path.radius?
		
		return seekPosition(target);
	}
};

class ofxPathFollowingFlock : public ofxFlock<FollowAgent> {
public:
	
	enum FollowType { NONE, TARGET_FOLLOW, PATH_FOLLOW };
	
	ofxPathFollowingFlock();
	
	virtual void update() override;
	
	void addPathCollection(FollowPathCollection &&collection) {
		mPathCollections.push_back(std::move(collection));
	}
	
	void assignAgentsToCollection(int index=0);
	
	
	ofParameter<float> mFollowAmount;
	ofParameter<int> mFollowType;
	
protected:
	std::vector<FollowPathCollection> mPathCollections;
	
};