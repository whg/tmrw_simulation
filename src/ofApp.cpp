#include "ofApp.h"

// based on Python from:
// http://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
vector<ofVec3f> fibonacciSpherePoints(size_t samples, float radius) {
    
    vector<ofVec3f> output;
    
    float offset = 2.0f / samples;
    float increment = PI * (3.0f - std::sqrt(5.0f));
    
    float x, y, z, r, phi;
    for (size_t i = 0; i < samples; ++i) {
        y = (((i * offset) - 1.0f) + offset * 0.5f);
        r = std::sqrt(1.0f - y * y);
        phi = i * increment;
        x = std::cos(phi) * r;
        z = std::sin(phi) * r;
        
        output.push_back(ofVec3f(x * radius, y * radius, z * radius));
    }
     
    return output;
}

void ofApp::setup(){

	ofSetWindowShape(1280, 720);
    float offset = 30;
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB32F);

    thing.load("spark3.png");
//    thing.setAnchorPercent(0.5, 0.5);
	
    fbo.begin();
    ofClear(0,0,0,0);
    fbo.end();
    
	svg.load("/Users/whg/Desktop/TMRW logo hexagon black.svg");

    mFont.load("Arial.ttf", 100, true, true, true);
    
    
    
//    for (int i = 0; i < 2000; i++) {
//		flock.addAgent(ofVec3f(ofRandom(-ofGetWidth(), ofGetWidth()), ofRandom(-ofGetWidth(), ofGetHeight()), ofRandom(-ofGetWidth(), ofGetHeight())));
//    }
	
	FollowPathCollection logo(svg, 5);
    logo.centerPoints(ofVec2f(0, 0)); //ofGetWidth()/2, ofGetHeight()/2));
	
    flock.addPathCollection(std::move(logo));
    
    FollowPathCollection fontCollection;
    fontCollection.add(mFont.getStringAsPoints("Hello"));
    fontCollection.centerPoints(ofVec2f(0, 0)); //ofGetWidth()/2, ofGetHeight()/2));
    
    flock.addPathCollection(std::move(fontCollection));
	
	flock.assignAgentsToCollection(1, true);

	
	ofBackground(0);
	
	gui.setup("");
	gui.add(flock.getSettings().maxSpeed);
	gui.add(flock.getSettings().maxForce);
	gui.add(flock.getSettings().cohesionDistance);
	gui.add(flock.getSettings().separationDistance);
	gui.add(flock.getSettings().cohesionAmount);
	gui.add(flock.getSettings().separationAmount);
	gui.add(flock.mDoFlock);
	gui.add(flock.mFollowAmount);
	gui.add(flock.mFollowType);
	gui.add(flock.getSettings().moveAlongTargets);
    gui.add(mAlpha.set("alpha", 10, 0, 200));
    gui.add(mImageSize.set("image size", 2, 1, 30));
    gui.add(mSphereSize.set("sphere size", 1, 0, 10));
    gui.add(mSphereIterations.set("sphere iterations", 1, 0, 100));
    
    mPathIndex.addListener(this, &ofApp::pathIndexChanged);
    gui.add(mPathIndex.set("path index", 0, 0, 2));

    ofSetLogLevel(OF_LOG_VERBOSE);

	p2lShader.load("points2lines.vert", "points2lines.frag", "points2lines.geom");

    flock.setup(ofGetWidth(), ofGetHeight(), 3);

}

void ofApp::exit() {
}

void ofApp::createAddressPoints() {

    auto points = fibonacciSpherePoints(mMacAddresses.size(), 200);
    size_t i = 0;
    for (auto &address : mMacAddresses) {
        address.second = points[i++];
    }
}


//--------------------------------------------------------------
void ofApp::update() {
    
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));

    mOscReceiver.update();
    auto &messages = mOscReceiver.getMessages();
    float timeNow = ofGetElapsedTimef();
    float timeDiff = mOscReceiver.getTimeDiff();
    
    int addedCount = 0;
    if (messages.size() > 0) {
        while (messages.size() > 0 && messages.front().time + timeDiff + 3 < timeNow) {
            auto &message = messages.back();
            
            if (mMacAddresses.count(message.from) == 0) {
                mMacAddresses.emplace(message.from, ofVec3f(0));
                createAddressPoints();
            }
            if (mMacAddresses.count(message.to) == 0) {
                mMacAddresses.emplace(message.to, ofVec3f(0));
                createAddressPoints();
            }
            
            auto pathKey = message.from + "-" + message.to;
            if (mMacPaths.count(pathKey) == 0) {
                auto path = make_shared<FollowPath>();
                path->addVertex(mMacAddresses.at(message.from));
                path->addVertex(mMacAddresses.at(message.to));
                mMacPaths.emplace(pathKey, path);
            }
            
            auto agent = make_shared<FollowAgent>(mMacAddresses.at(message.from), flock.getSettings());
            agent->set(mMacPaths.at(pathKey), 1, mMacAddresses.at(message.to));
            flock.addAgent(agent);
            
            messages.pop_front();
            
            
            cout << "added agent for " << message.time + timeDiff  << ", " << message.time << ", " << timeNow << ", " << timeDiff << endl;
            cout << mMacAddresses.at(message.from) << "-->" << mMacAddresses.at(message.to) << endl;
            addedCount++;
        }
    }
    
    printf("added %d, timeNow: %f, size = %d\n", int(addedCount), timeNow, int(messages.size()));
    
    flock.cleanUpArrivedAgents();
    
//    cout << addedCount << " : " << timeNow  << endl;
}

//--------------------------------------------------------------
void ofApp::draw(){

	auto start = ofGetElapsedTimef();
	
	const auto &agents = flock.getAgents();
    
    ofMesh lineMesh;
    lineMesh.setMode(OF_PRIMITIVE_LINES);
    
    float hue = 0;
    
//    for (auto agent : flock.mAgents) {
//
//        auto pushAgents = flock.getNeighbours(agent->mPos, 15, 23);
//        for (const auto *oAgent : pushAgents) {
//            lineMesh.addVertex(agent->mPos);
//            lineMesh.addVertex(oAgent->mPos);
//            lineMesh.addColor(ofFloatColor::fromHsb(hue, 1, 1));
//            lineMesh.addColor(ofFloatColor::fromHsb(hue, 1, 1));
//
////            hue+= 0.01;
//            if (hue > 1) hue-= 1;
//        }
//
//        
////        flock.addForce(agent->mPos, 10, 0.1);
//        
//        
//        agent->damp();
//    }
//	
    fbo.begin();
    
    ofSetColor(0, 0, 0, mAlpha);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
//    ofBackground(0);

    
        cam.begin();

    
//    
//    auto points = fibonacciSpherePoints(mSphereIterations, mSphereSize);
//    ofMesh pMesh;
//    pMesh.addVertices(points);
////    pMesh.draw(OF_MESH_POINTS);
//
    ofNoFill();
    ofIcoSpherePrimitive sphere(mSphereSize, mSphereIterations);
//

    ofSetColor(255);
    for (auto &pair : mMacAddresses) {
        ofPushMatrix();
        ofTranslate(pair.second);
        sphere.drawWireframe();
        ofPopMatrix();
    }

    ofMesh mesh;
    hue = 0.4;
    for (auto agent : flock.mAgents) {
        
        mesh.addVertex(agent->mPos);
        mesh.addVertex(agent->mPos + ofVec2f(mImageSize, 0));
        mesh.addColor(ofFloatColor::fromHsb(hue, 1, 1));
        mesh.addColor(ofFloatColor::fromHsb(hue, 1, 1));
        hue+= 0.001;
        if (hue > 0.9) hue-= 0.5;
        
        ofSetColor(ofFloatColor::fromHsb(hue, 1, 1));

    }
    
    flock.update();

    
    
    p2lShader.begin();
    p2lShader.setUniformTexture("tex", thing, 0);
    p2lShader.setUniform2f("texSize", thing.getWidth(), thing.getHeight());
    
    
    mesh.setMode(OF_PRIMITIVE_LINES);
    mesh.draw(OF_MESH_FILL);

    
    p2lShader.end();
    

    cam.end();
    fbo.end();

    fbo.draw(0, 0);

	gui.draw();

    ofSetColor(255);
    ofDrawBitmapString(ofToString(flock.getAgents().size()), 10, ofGetHeight() - 12);
	
}


void ofApp::keyPressed(int key) {
}

void ofApp::pathIndexChanged(int &index) {
    flock.assignAgentsToCollection(index, true);
}