#include "ofApp.h"
using namespace whg;

float rms;
float ampv = 0;
float t = 0;
int b = 20;
bool gobig = false;

float midThres = 0.4;
float redSep = 1.0;

int skip = 2;
bool play = true;
ofPoint mid;

float adata[512];

//--------------------------------------------------------------
void ofApp::setup(){

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

	svg.load("/Users/whg/Desktop/tomorrow_logo_t.svg");
    for (int i = 0; i < svg.getNumPath(); i++){
        ofPath &p = svg.getPathAt(i);
        // svg defaults to non zero winding which doesn't look so good as contours
        		p.setPolyWindingMode(OF_POLY_WINDING_ODD);
        const vector<ofPolyline>& lines = p.getOutline();
        for(int j=0;j<(int)lines.size();j++){
			outlines.push_back(lines[j]);//.getResampledBySpacing(20));
            paths.push_back(whg::Path());
            paths[i].addVertices(lines[j].getResampledBySpacing(5).getVertices());
        }
		
        vector<ofPoint> &pts = paths[i].getVertices();
        for(int j=0;j<pts.size();j++){
            pts[j].x +=ofGetWidth()/2 - svg.getWidth()/2;
            pts[j].y += ofGetHeight()/2 - svg.getHeight()/2;
        }
    }
    
    
    for (int i = 0; i < 1000; i++) {
        //        newVehicle(random(width),random(height));
//        points.push_back(new Vehicle());

		flock.addAgent(ofVec3f(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()), 0));

    }
	
	flock.addPathCollection(FollowPathCollection(svg, 5));
	
	flock.assignAgentsToCollection(0);
	
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
	gui.add(flock.mFollowAmount);
}

void ofApp::sineWavePoints() {
    float padding = 250;
    float amp = (ofGetHeight() - padding*2) * 0.5;
    float xstep = (ofGetWidth() - padding*2.0) / points.size();
    cout << xstep << endl;
    float y = ofGetHeight() * 0.5;
    float x = padding;
    float amult =  TWO_PI / points.size();
    for (int i = 0; i < points.size(); i++) {
        points[i]->location.x = x + i* xstep;
        points[i]->location.y = y + sin(i*amult*2)*amp;
    }
}

void ofApp::circlePoints() {
    float padding = 250;
    float amp = (ofGetHeight() - padding*2) * 0.5;
    float xstep = (ofGetWidth() - padding*2.0) / points.size();
    cout << xstep << endl;
    float y = ofGetHeight() * 0.5;
    float x = ofGetWidth() * 0.5;
    float amult =  TWO_PI / points.size();
    for (int i = 0; i < points.size(); i++) {
        points[i]->location.x = x + cos(i*amult)*amp*2;
        points[i]->location.y = y + sin(i*amult*circleMult)*amp;
    }
    circleMult++;
    
}


void ofApp::linePoints() {
    float padding = 250;
    float xstep = (ofGetWidth() - padding*2.0) / points.size();
    cout << xstep << endl;
    float y = ofGetHeight() * 0.5;
    float x = padding;
    float amult =  TWO_PI / points.size();
    for (int i = 0; i < points.size(); i++) {
        points[i]->location.x = x + i* xstep;
        points[i]->location.y = y;
    }
}


void ofApp::randomisePoints() {
    for (int i = 0; i < points.size(); i++) {
        //        newVehicle(random(width),random(height));
//        points.push_back(new Vehicle());
        int pathIndex = i%paths[i%paths.size()].size();
        points[i]->target = paths[i%paths.size()].getVertices()[pathIndex];
        points[i]->currentPointIndex = pathIndex;
        points[i]->path = paths[i%paths.size()];
        int w2 = ofGetWidth() /2;
        int h2 = ofGetHeight() /2;
        points[i]->location = ofVec3f(ofRandom(0, w2*2), ofRandom(0, h2*2));
    }

}

void ofApp::exit() {
    for (int i = 0; i < points.size(); i++) {
        delete points[i];
    }
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
	
	for (const auto &agent : agents) {
		
		thing.draw(agent->mPos, b, b);
//		ofDrawRectangle(agent->mPos, b, b);
	}
	
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
	

	cout << (ofGetElapsedTimef() - start) * 1000 << ", ";
	
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
        skip = MAX(2, skip--);
    }
    if(key == OF_KEY_RIGHT) {
        skip = MIN(250, skip++);
    }
//    cout << "skip = " << skip << endl;
    if(key == 'a') play = true;
    
    
    if(key == 'r') randomisePoints();
    
    if(key == 'a') {
//        cout << "aaaa" << endl;
        ampv += 100;
    }
    if(key == '0') ampv = 0;
    
    if(key == 'q') midThres+= 0.1;
    if(key == 'a') midThres-=0.1;
    if(key == 'w') circleThresh+= 0.01;
    if(key == 's') circleThresh-= 0.01;
    
    if(key == 'k') sineWavePoints();
    if(key == 'l') linePoints();
    if(key == ';') circlePoints();
    if(key == 'z') circleMult = 0;
    if(key == 'p') doCirclePoints = !doCirclePoints;
    
    if(key == 'f' || key == 'G') doMove = !doMove;
    
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