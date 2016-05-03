#include "ofApp.h"

void ofApp::setup(){

	ofSetWindowShape(640, 480);
    float offset = 30;
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB32F);

	thing.load("spark3.png");
    thing.setAnchorPercent(0.5, 0.5);
	
    fbo.begin();
    ofClear(0,0,0,0);
    fbo.end();
    
	svg.load("/Users/whg/Desktop/TMRW logo hexagon black.svg");

    
    for (int i = 0; i < 1000; i++) {
		flock.addAgent(ofVec3f(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()), 0));

    }
	
	FollowPathCollection logo(svg, 5);
	logo.centerPoints(ofVec2f(ofGetWidth()/2, ofGetHeight()/2));
	flock.addPathCollection(std::move(logo));
	
	flock.assignAgentsToCollection(0, true);

	
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


//	p2lShader.setGeometryInputType(GL_POINTS);
//	p2lShader.setGeometryOutputType(GL_POINTS);
//	p2lShader.setGeometryOutputCount(1);

//	p2lShader.load("points2lines.vert", "points2lines.frag");//, "points2lines.geom");
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
	
	flock.getSettings().separationAmount = ofMap(mouseX, 0, ofGetWidth(), 0, 3);
	flock.getSettings().cohesionAmount = ofMap(mouseY, 0, ofGetHeight(), 0, 3);
	
    
    flock.fillBins();
	
	const auto &agents = flock.getAgents();
    
    ofMesh lineMesh;
    lineMesh.setMode(OF_PRIMITIVE_LINES);
    
    for (auto agent : flock.mAgents) {

        auto pushAgents = flock.getNeighbours(agent->mPos, 15);
        size_t counter = 0;
        for (const auto *oAgent : pushAgents) {
            lineMesh.addVertex(agent->mPos);
            lineMesh.addVertex(oAgent->mPos);
            if (++counter >= 5) {
                break;
            }
        }

        
//        flock.addForce(agent->mPos, 10, 0.1);
        
        
        agent->damp();
    }
	
//    flock.addAttractionForce(ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2), 400, 0.01);
    if(ofGetMousePressed()) {
        flock.addRepulsionForce(ofVec2f(mouseX, mouseY), 50, 1);
    }
    
    ofMesh mesh;
    for (auto agent : flock.mAgents) {
        
        mesh.addVertex(agent->mPos);
//        agent->update();
    }
    
    flock.update();

    
    mesh.draw(OF_MESH_POINTS);
    

    lineMesh.draw(OF_MESH_WIREFRAME);
    
//	fbo.begin();
//	
////	p2lShader.set
//	
//	ofSetColor(0, 0, 0, alpha);
//	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
//
////	ofBackground(120);
//	
//	ofEnableBlendMode(OF_BLENDMODE_ADD);
////	p2lShader.begin();
////	p2lShader.bindDefaults();
////
//    ofSetColor(255);
//
//	ofMesh mesh;
//	int i = 0;
//	for (const auto &agent : agents) {
//		
////		ofSetColor(200, 100, i % 255);
//		i++;
//		thing.draw(agent->mPos, b, b);
////		mesh.addVertex(agent->mPos);
////		ofDrawRectangle(agent->mPos, b, b);
//	}
////
//	
////	mesh.draw(OF_MESH_POINTS);
//
////	ofDrawLine(100, 100, 300, 100);
//	
//	
////	p2lShader.end();
//	fbo.end();
//	fbo.draw(0, 0);

	gui.draw();

	

	
}
