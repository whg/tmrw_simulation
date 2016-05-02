#include "ofApp.h"

float rms;
float ampv = 0;
float t = 0;
int b = 20;
bool gobig = false;

float midThres = 0.4;
float redSep = 1.0;

int alpha = 200;
bool play = true;
ofPoint mid;

float adata[512];

//--------------------------------------------------------------
void ofApp::setup(){
//	
	ofSetWindowShape(640, 480);
//    path = new Path();
    float offset = 30;

//    ofSetFrameRate(30);
    circleThresh = 0.04;
    randAmount = 0;
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB32F);
//    thing.loadImage("thing.png");

	thing.load("spark3.png");
    thing.setAnchorPercent(0.5, 0.5);
//    ofSetFrameRate(30);
//    ofSetVerticalSync(true);
//    ofSetCircleResolution(100);
	
    fbo.begin();
    ofClear(0,0,0,0);
    fbo.end();
    
//    svg.load("unleash.svg");

	svg.load("/Users/whg/Desktop/TMRW logo hexagon black.svg");

    
    for (int i = 0; i < 1000; i++) {
        //        newVehicle(random(width),random(height));
//        points.push_back(new Vehicle());

		flock.addAgent(ofVec3f(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()), 0));

    }
	
	FollowPathCollection logo(svg, 5);
	logo.centerPoints(ofVec2f(ofGetWidth()/2, ofGetHeight()/2));
	flock.addPathCollection(std::move(logo));
	
	flock.assignAgentsToCollection(0, true);
	
//    randomisePoints();
//    sample.load(ofToDataPath("maintain-short.wav"));
//    
//    ofSoundStreamSetup(2, 0, 44100, 512, 2);

    mid = ofPoint(ofGetWidth()/2 - svg.getWidth()/2, ofGetHeight()/2 - svg.getHeight()/2);
	doMove = true;
	
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
//
	ofSetLogLevel(OF_LOG_VERBOSE);

//	p2lShader.setGeometryInputType(GL_POINTS);
//	p2lShader.setGeometryOutputType(GL_POINTS);
//	p2lShader.setGeometryOutputCount(1);

//	p2lShader.load("points2lines.vert", "points2lines.frag");//, "points2lines.geom");
//	ofLog() << "Maximum number of output vertices support is: " << p2lShader.getGeometryMaxOutputCount();

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
	
	flock.update();
	
	const auto &agents = flock.getAgents();
	
	fbo.begin();
	
//	p2lShader.set
	
	ofSetColor(0, 0, 0, alpha);
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

//	ofBackground(120);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
//	p2lShader.begin();
//	p2lShader.bindDefaults();
//
    ofSetColor(255);

	ofMesh mesh;
	int i = 0;
	for (const auto &agent : agents) {
		
//		ofSetColor(200, 100, i % 255);
		i++;
		thing.draw(agent->mPos, b, b);
//		mesh.addVertex(agent->mPos);
//		ofDrawRectangle(agent->mPos, b, b);
	}
//
	
//	mesh.draw(OF_MESH_POINTS);

//	ofDrawLine(100, 100, 300, 100);
	
	
//	p2lShader.end();
	fbo.end();
	fbo.draw(0, 0);

	gui.draw();

	
//	int i = 0;
//    for (vector<Vehicle*>::iterator it = points.begin(); it != points.end(); ++it) {
//
//		if (gobig) {
//			(*it)->followPath(points);
//		}
//		
//        // Path following and separation are worked on in this function
//        if (doCirclePoints) {
//			(*it)->follow(paths[i%paths.size()]);
//			i++;			
//		}
//		
//        if(doMove) {
//			
//			if (play) {
//            (*it)->flock(points, ofMap(mouseX, 0, ofGetWidth(), 0, 3),
//								 ofMap(mouseY, 0, ofGetHeight(), 0, 3));
//			}
//			
//			(*it)->update();
//
//        }
//		
//		
//		thing.draw((*it)->location, b, b); //mouseX, mouseX);
//    }
	

//	cout << (ofGetElapsedTimef() - start) * 1000 << ", ";
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == OF_KEY_UP) {
        b++;
    }
    if (key == OF_KEY_DOWN) {
        b--;
    }
    if(key == OF_KEY_LEFT) {
        alpha = MAX(1, alpha--);
    }
    if(key == OF_KEY_RIGHT) {
        alpha = MIN(250, alpha++);
    }
//    cout << "alpha = " << alpha << endl;
    if(key == 'a') play = true;
    
    
	
    if(key == 'a') {
//        cout << "aaaa" << endl;
        ampv += 100;
    }
    if(key == '0') ampv = 0;
    
    if(key == 'q') midThres+= 0.1;
    if(key == 'a') midThres-=0.1;
    if(key == 'w') circleThresh+= 0.01;
    if(key == 's') circleThresh-= 0.01;
    
	
    if(key == 'f' || key == 'G') doMove = !doMove;
	
	if (key == ' ') {
		flock.assignAgentsToCollection(0, true);
	}
	
//    cout << "midThesh = " << midThres;
//    cout << " circle thres " << circleThresh << endl;
	
//    if (key == 'c') {
//        for (vector<Vehicle*>::iterator it = points.begin(); it != points.end(); ++it) {
//            
//            (*it)->r = 10;
//            
//        }
//    }
//    
//    
//    if (key == 'x') {
//        for (vector<Vehicle*>::iterator it = points.begin(); it != points.end(); ++it) {
//            
//            (*it)->r = 2;
//            
//        }
//    }
    
    if (key == 'c') {
        gobig = !gobig;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 'a') play = false;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    randAmount = x;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    randAmount = x;
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    randAmount = 0;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}