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
	mAgentSettings.moveAlongTargets.set("move along", false);
	
	mDoFlock.set("do flock", false);
	
	mCacheThread = std::thread(&ofxFlock::calcCaches, this);
	mCacheThread.detach();
//	mCalcedCaches.store(false);
}

template <class AgentType>
void ofxFlock<AgentType>::addAgent(ofVec3f pos) {
	
	mPositions.push_back(pos);
	auto agent = make_shared<AgentType>(mPositions.back(), mAgentSettings);
	mAgents.push_back(std::move(agent));
	mNAgents = mPositions.size();
	
	mCohesionCache.resize(mNAgents);
	mSeparationCache.resize(mNAgents);
}

template <class AgentType>
void ofxFlock<AgentType>::update() {

	for (size_t i = 0; i < mNAgents; i++) {
		auto agent = mAgents[i];
		
		if (mDoFlock) {
		
//			std::unique_lock<std::mutex> locker(mCacheMutex);
			
			auto &cohesionData = mCohesionCache.getFrontData();
			auto &cohesionCounts = mCohesionCache.getFrontCounts();
			
			auto &separationData = mSeparationCache.getFrontData();
			auto &separationCounts = mSeparationCache.getFrontCounts();
		
//			locker.unlock();
		
			auto cohesionCount = static_cast<float>(cohesionCounts[i]);
			if (cohesionCount > 0) {
				auto cohesionForce = agent->seekPosition(cohesionData[i] / cohesionCount);
				
				agent->apply(cohesionForce * mAgentSettings.cohesionAmount);
			}

			auto separationCount = static_cast<float>(separationCounts[i]);
			if (separationCount > 0) {
				auto separationForce = agent->seek(separationData[i] / separationCount);
				
				agent->apply(separationForce * mAgentSettings.separationAmount);
			}
			
//			mCalcedCaches.store(false);

			mCaclCachesCondition.notify_one();

		}
		
		agent->update();
	}

}

template <class AgentType>
void ofxFlock<AgentType>::calcCaches() {
	while (true) {
//		while (!mCalcedCaches.load()) {
		
        auto start = ofGetElapsedTimef();

        float distSquared;
        
        float cohestionDistance = std::pow(mAgentSettings.cohesionDistance, 2);
        float separationDistance = std::pow(mAgentSettings.separationDistance, 2);
        
        auto &cohesionData = mCohesionCache.getBackData();
        auto &cohesionCounts = mCohesionCache.getBackCounts();
        
        auto &separationData = mSeparationCache.getBackData();
        auto &separationCounts = mSeparationCache.getBackCounts();
        
        for (size_t i = 0; i < mNAgents; i++) {
            cohesionData[i].set(0);
            cohesionCounts[i] = 0;
            separationData[i].set(0);
            separationCounts[i] = 0;
        }
        
        
        for (size_t i = 0; i < mNAgents; i++) {
            
            auto &positionI = mAgents[i]->mPos;
            
            for (size_t j = i + 1; j < mNAgents; j++) {
                
                auto &positionJ = mAgents[j]->mPos;
                
                distSquared = positionI.squareDistance(positionJ);
                
                if (distSquared < cohestionDistance) {
                    cohesionData[i]+= positionJ;
                    cohesionCounts[i]++;
                    
                    cohesionData[j]+= positionI;
                    cohesionCounts[j]++;
                    
                }
                
                if (distSquared < separationDistance) {
                    separationData[i]+= (positionI - positionJ);
                    separationCounts[i]++;
                    
                    separationData[j]+= (positionJ - positionI);
                    separationCounts[j]++;
                }
            }
        }
        
        
//			mCalcedCaches.store(true);
        std::unique_lock<std::mutex> locker(mCacheMutex);
        mCaclCachesCondition.wait(locker);
        
        mCohesionCache.swap();
        mSeparationCache.swap();
			
//		} // end mCalcedCaches == false
	} // end infinite
}

void ofxFlock::populateMesh(ofMesh &mesh, FlockMeshSettings settings) {
    
    
}

template class ofxFlock<Agent>;
template class ofxFlock<FollowAgent>;

ofxPathFollowingFlock::ofxPathFollowingFlock() {
	mFollowAmount.set("followAmount", 1, 0, 6);
	mFollowType.set("follow type", 0, 0, 2);
}

void ofxPathFollowingFlock::update() {
	
	if (mFollowType != NONE) {
		if (mFollowType == TARGET_FOLLOW) {
			for (auto agent : mAgents) {
				if (agent->mPath) {
					agent->apply(agent->moveToNextTarget() * mFollowAmount);
				}
			}
		}
		else if (mFollowType == PATH_FOLLOW) {
			for (auto agent : mAgents) {
				if (agent->mPath) {
					agent->apply(agent->moveAlongPath() * mFollowAmount);
				}
			}
		}
	}
	
	ofxFlock<FollowAgent>::update();
}

void ofxPathFollowingFlock::assignAgentsToCollection(int index, bool assignIndividual) {
	
	assert(index >= 0 && index < mPathCollections.size());
	
	auto &collection = mPathCollections[index];
	
	if (assignIndividual) {
		float totalLength = collection.getTotalLength();
		auto distancePerAgent = totalLength / mNAgents;
		collection.resampleBySpacing(distancePerAgent);
	}
	
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

