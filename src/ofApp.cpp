#include "ofApp.h"



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
    
    
    
    
    for (int i = 0; i < 2000; i++) {
		flock.addAgent(ofVec3f(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()), 0));

    }
	
	FollowPathCollection logo(svg, 5);
	logo.centerPoints(ofVec2f(ofGetWidth()/2, ofGetHeight()/2));
	
    flock.addPathCollection(std::move(logo));
    
    FollowPathCollection fontCollection;
    fontCollection.add(mFont.getStringAsPoints("Hello"));
    fontCollection.centerPoints(ofVec2f(ofGetWidth()/2, ofGetHeight()/2));
    
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
    
    mPathIndex.addListener(this, &ofApp::pathIndexChanged);
    gui.add(mPathIndex.set("path index", 0, 0, 2));

    ofSetLogLevel(OF_LOG_VERBOSE);
//	p2lShader.setGeometryInputType(GL_LINES);
//	p2lShader.setGeometryOutputType(GL_TRIANGLE_STRIP);
//	p2lShader.setGeometryOutputCount(4);

	p2lShader.load("points2lines.vert", "points2lines.frag", "points2lines.geom");
//	ofLog() << "Maximum number of output vertices support is: " << p2lShader.getGeometryMaxOutputCount();

    flock.setup(ofGetWidth(), ofGetHeight(), 3);

}

void ofApp::exit() {
}


//--------------------------------------------------------------
void ofApp::update(){
    
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));

}

//--------------------------------------------------------------
void ofApp::draw(){

	auto start = ofGetElapsedTimef();
	
//	flock.getSettings().separationAmount = ofMap(mouseX, 0, ofGetWidth(), 0, 3);
//	flock.getSettings().cohesionAmount = ofMap(mouseY, 0, ofGetHeight(), 0, 3);
	
    
    flock.fillBins();
	
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
//    cam.begin();
    fbo.begin();

    
    if(ofGetMousePressed()) {
        flock.addRepulsionForce(ofVec2f(mouseX, mouseY), 50, 1);
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
//        thing.draw(agent->mPos, mImageSize, mImageSize);
    }
    
    flock.update();

    
    
    ofSetColor(0, 0, 0, mAlpha);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
//    ofEnableNormalizedTexCoords();
    thing.bind();
//
    
    p2lShader.begin();
    p2lShader.setUniformTexture("tex", thing, 0);
    p2lShader.setUniform2f("texSize", thing.getWidth(), thing.getHeight());
    
    ofSetColor(255);
    
    mesh.setMode(OF_PRIMITIVE_LINES);
    mesh.draw(OF_MESH_FILL);
    
    p2lShader.end();
    
    thing.unbind();
    
//    ofSetColor(100);
//    lineMesh.draw(OF_MESH_WIREFRAME);
    fbo.end();
//
    fbo.draw(0, 0);
//    cam.end();
//
	gui.draw();

	
//    thing.draw(0, 200);
	
}


void ofApp::keyPressed(int key) {
}

void ofApp::pathIndexChanged(int &index) {
    flock.assignAgentsToCollection(index);
}