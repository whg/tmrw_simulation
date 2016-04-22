//
//  ofxFlock.cpp
//  emptyExample
//
//  Created by Will Gallia on 22/04/2016.
//
//

#include "ofxFlock.h"
#include "ofMain.h"

template <class AgentType>
ofxFlock<AgentType>::ofxFlock() {
	mAgentSettings.maxSpeed.set("maxSpeed", 1, 0.001, 5);
	mAgentSettings.maxForce.set("maxForce", 0.1, 0.001, 3);
	mAgentSettings.cohesionDistance.set("cohesionDistance", 200, 10, 500);
	mAgentSettings.separationDistance.set("separationDistance", 20, 10, 500);
	mAgentSettings.cohesionAmount.set("cohesionAmount", 1, 0, 3);
	mAgentSettings.separationAmount.set("separationAmount", 1, 0, 3);
	
	
	mCacheThread = std::thread(&ofxFlock::calcCaches, this);
	mCacheThread.detach();
	mCalcedCaches.store(false);
}

template <class AgentType>
void ofxFlock<AgentType>::addAgent(ofVec3f pos) {
	
	mPositions.push_back(pos);
	auto agent = make_shared<AgentType>(mPositions.back(), mAgentSettings);
	mAgents.push_back(std::move(agent));
	mNAgents = mPositions.size();
	
	mCohesionCache.resize(mNAgents);
	mSeparationCache.resize(mNAgents);
	
	mCohesionCacheCounts.resize(mNAgents);
	mSeparationCacheCounts.resize(mNAgents);
}

template <class AgentType>
void ofxFlock<AgentType>::update() {

	for (size_t i = 0; i < mNAgents; i++) {
		auto agent = mAgents[i];
		
		auto cohesionCount = static_cast<float>(mCohesionCacheCounts[i]);
		if (cohesionCount > 0) {
			auto cohesionForce = agent->seekPosition(mCohesionCache[i] / cohesionCount);
			
			agent->apply(cohesionForce * mAgentSettings.cohesionAmount);
		}

		auto separationCount = static_cast<float>(mSeparationCacheCounts[i]);
		if (separationCount > 0) {
			auto separationForce = agent->seek(mSeparationCache[i] / separationCount);
			
			agent->apply(separationForce * mAgentSettings.separationAmount);
		}
		
		agent->update();
	}

	mCalcedCaches.store(false);
}

template <class AgentType>
void ofxFlock<AgentType>::calcCaches() {
	while (true) {
		while (!mCalcedCaches.load()) {
			
			auto start = ofGetElapsedTimef();

			float distSquared;
			
			float cohestionDistance = std::pow(mAgentSettings.cohesionDistance, 2);
			float separationDistance = std::pow(mAgentSettings.separationDistance, 2);
			
			for (size_t i = 0; i < mNAgents; i++) {
				mCohesionCache[i].set(0);
				mCohesionCacheCounts[i] = 0;
				mSeparationCache[i].set(0);
				mSeparationCacheCounts[i] = 0;
			}
			
			
			for (size_t i = 0; i < mNAgents; i++) {
				
				auto &positionI = mAgents[i]->mPos;
				
				for (size_t j = i + 1; j < mNAgents; j++) {
					
					auto &positionJ = mAgents[j]->mPos;
					
					distSquared = positionI.squareDistance(positionJ);
					
					if (distSquared < cohestionDistance) {
						mCohesionCache[i]+= positionJ;
						mCohesionCacheCounts[i]++;
						
						mCohesionCache[j]+= positionI;
						mCohesionCacheCounts[j]++;
						
					}
					
					if (distSquared < separationDistance) {
						mSeparationCache[i]+= (positionI - positionJ);
						mSeparationCacheCounts[i]++;
						
						mSeparationCache[j]+= (positionJ - positionI);
						mSeparationCacheCounts[j]++;
					}
				}
			}
			
			
			mCalcedCaches.store(true);

		} // end mCalcedCaches == false
	} // end infinite
}

template class ofxFlock<Agent>;
template class ofxFlock<FollowAgent>;

ofxPathFollowingFlock::ofxPathFollowingFlock() {
	mFollowAmount.set("followAmount", 1, 0, 3);
}

void ofxPathFollowingFlock::update() {
	
	for (auto agent : mAgents) {
		if (agent->mPath) {
			agent->apply(agent->moveAlongPath() * mFollowAmount);
			agent->update();
		}
	}
	
//
//ofxFlock<FollowAgent>::update();
}

void ofxPathFollowingFlock::assignAgentsToCollection(int index) {
	
	assert(index >= 0 && index < mPathCollections.size());
	
	auto &collection = mPathCollections[index];
	
	auto totalVerts = collection.getTotalVertices();
	
	if (totalVerts > mAgents.size()) {
	
		size_t agentCounter = 0;
		for (auto pathPtr : collection.mPaths) {
			size_t vertexCounter = 0;
			for (auto &vertex : pathPtr->getVertices()) {
				auto &agent = mAgents[agentCounter++];
				agent->mPath = pathPtr;
				agent->mTargetIndex = vertexCounter++;
				
				if (agentCounter >= mAgents.size()) {
					break;
				}
			}
			if (agentCounter >= mAgents.size()) {
				break;
			}

		}
	}
	else {
		size_t vertexIndex = 0, pathIndex = 0;
		for (size_t i = 0; i < mAgents.size(); i++) {
			auto pathPtr = collection.mPaths[pathIndex];
			auto &vertex = pathPtr->getVertices()[vertexIndex];
			
			auto &agent = mAgents[i];
			agent->mPath = pathPtr;
			agent->mTargetIndex = vertexIndex;
			
			if (++vertexIndex == pathPtr->size()) {
				pathIndex = (pathIndex + 1) % collection.mPaths.size();
				vertexIndex = 0;
			}
		}
	}
}

