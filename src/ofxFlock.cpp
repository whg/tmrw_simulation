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
	
//	mCacheThread = std::thread(&ofxFlock::calcCaches, this);
//	mCacheThread.detach();
}

template <class AgentType>
void ofxFlock<AgentType>::setup(size_t width, size_t height, size_t binShift) {
    
    float binSize = 1 << binShift;
    mXBins = std::ceil(width / binSize);
    mYBins = std::ceil(height / binSize);
    mBins.resize(mXBins * mYBins);
    mBinShift = binShift;
}

template <class AgentType>
void ofxFlock<AgentType>::fillBins() {

    for (auto &bin : mBins) {
        bin.clear();
    }
    
    size_t xBin, yBin;
    for (const auto agent : mAgents) {
        agent->reset();
        auto &pos = agent->mPos;
        xBin = static_cast<size_t>(pos.x) >> mBinShift;
        yBin = static_cast<size_t>(pos.y) >> mBinShift;
        if (xBin < mXBins && yBin < mYBins) {
            mBins[yBin * mXBins + xBin].push_back(agent.get());
        }
    }
}

template<class AgentType>
std::vector<const AgentType*> ofxFlock<AgentType>::getRegion(ofRectangle &region) {
    
    vector<const AgentType*> output;
    back_insert_iterator<vector<const AgentType*>> inserter = back_inserter(output);
    size_t minXBin = static_cast<size_t>(region.getLeft()) >> mBinShift;
    size_t maxXBin = std::min((static_cast<size_t>(region.getRight()) >> mBinShift) + 1, mXBins);
    size_t minYBin = static_cast<size_t>(region.getBottom()) >> mBinShift;
    size_t maxYBin = std::min((static_cast<size_t>(region.getTop()) >> mBinShift) + 1, mYBins);
    
    for (size_t i = minXBin; i < maxXBin; i++) {
        for (size_t j = minYBin; j < maxYBin; j++) {
            auto &group = mBins[j * mXBins + i];
            std::copy(group.begin(), group.end(), inserter);
        }
    }
    return output;

}

template<class AgentType>
std::vector<const AgentType*> ofxFlock<AgentType>::getNeighbours(ofVec2f pos, float radius) {

    ofRectangle region;
    region.setFromCenter(pos, radius * 2, radius * 2);
    auto neighbours = getRegion(region);
    
    vector<const AgentType*> output;
    float squaredRadius = radius * radius;
    ofVec2f distance;
    for (const AgentType *agent : neighbours) {
        if ((agent->mPos - pos).lengthSquared() < squaredRadius) {
            output.push_back(agent);
        }
    }
    return output;
}

template <class AgentType>
void ofxFlock<AgentType>::addAgent(ofVec3f pos) {
	
	auto agent = make_shared<AgentType>(pos, mAgentSettings);
	mAgents.push_back(std::move(agent));
	mNAgents = mAgents.size();
	
	mCohesionCache.resize(mNAgents);
	mSeparationCache.resize(mNAgents);
}

template <class AgentType>
void ofxFlock<AgentType>::addRepulsionForce(ofVec2f pos, float radius, float amount) {
    addForce(pos, radius, amount);
}

template <class AgentType>
void ofxFlock<AgentType>::addAttractionForce(ofVec2f pos, float radius, float amount) {
    addForce(pos, radius, -amount);
}

template <class AgentType>
void ofxFlock<AgentType>::addForce(ofVec2f pos, float radius, float amount) {
    float minX = std::max(0.0f, pos.x - radius);
    float minY = std::max(0.0f, pos.y - radius);
    float maxX = pos.x + radius;
    float maxY = pos.y + radius;

    size_t minXBin = static_cast<size_t>(minX) >> mBinShift;
    size_t minyBin = static_cast<size_t>(minY) >> mBinShift;
    size_t maxXBin = std::min((static_cast<size_t>(maxX) >> mBinShift) + 1, mXBins);
    size_t maxYBin = std::min((static_cast<size_t>(maxY) >> mBinShift) + 1, mYBins);

    ofVec3f diff;
    float distanceSquared, distance, maxrsq;
    float effect;
    float radiusSquared = radius * radius;
    
    for (size_t i = minXBin; i < maxXBin; i++) {
        for (size_t j = minyBin; j < maxYBin; j++) {
            auto &bin = mBins[j * mXBins + i];
            for (const AgentType *agent: bin) {
                diff = agent->mPos - pos;
                distanceSquared = (agent->mPos - pos).lengthSquared();
                if (distanceSquared > 0 && distanceSquared < radiusSquared) {
                    distance = std::sqrt(distanceSquared);
                    diff /= distance;
                    effect = (1 - (distance / radius)) * amount;
                    agent->mAcc += diff * effect;
                }
            }
        }
    }

}


template <class AgentType>
void ofxFlock<AgentType>::update() {

	for (size_t i = 0; i < mNAgents; i++) {
		auto agent = mAgents[i];
		
		if (mDoFlock) {
					
			auto &cohesionData = mCohesionCache.getFrontData();
			auto &cohesionCounts = mCohesionCache.getFrontCounts();
			
			auto &separationData = mSeparationCache.getFrontData();
			auto &separationCounts = mSeparationCache.getFrontCounts();
		
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
			
			mCaclCachesCondition.notify_one();

		}
		
		agent->update();
	}

}

template <class AgentType>
void ofxFlock<AgentType>::calcCaches() {
	while (true) {
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
        
        
        std::unique_lock<std::mutex> locker(mCacheMutex);
        mCaclCachesCondition.wait(locker);
        
        mCohesionCache.swap();
        mSeparationCache.swap();
			
	} // end infinite
}

template <class AgentType>
void ofxFlock<AgentType>::populateMesh(ofMesh &mesh, FlockMeshSettings settings) {
    
    
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

