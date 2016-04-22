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
    
    
    for (int i = 0; i < 2000; i++) {
        //        newVehicle(random(width),random(height));
        points.push_back(new Vehicle());
    }
    randomisePoints();
//    sample.load(ofToDataPath("maintain-short.wav"));
//    
//    ofSoundStreamSetup(2, 0, 44100, 512, 2);

    mid = ofPoint(ofGetWidth()/2 - svg.getWidth()/2, ofGetHeight()/2 - svg.getHeight()/2);
	doMove = true;
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

//    path.display();
//    fbo.begin();
	
	auto start = ofGetElapsedTimef();
	
	ofBackground(0);
	ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    //	glColor4f(0, 0, 0, 1.0);
    ofFill();
    ofSetColor(0, 0, 0, b);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    //    ofClear(0,0,0,0.01);
    
    glColor4f(0.3, 0.3, 0.3,0.1);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    int iterations = 1;
    
    
    //    path.display();
    
//    ofTranslate(mid.x, mid.y);

    ofPushMatrix();
    ofTranslate(50, 250);
    ofSetColor(255);
    for(int i = 0; i < 512; i++) {
        ofLine(i, 0, i, adata[i]*200);
    }
    ofPopMatrix();
    
//    ofSetHexColor(0xff08ff);
    ofNoFill();
    for (int i = 0; i < (int)outlines.size(); i++){
        ofPolyline & line = outlines[i];
        
        int num = 1 * line.size();
        
        glBegin(GL_POINTS);
//        ofBeginShape();
        for (int j = 0; j < num; j++){
//            ofVertex(line[j]);
//            glVertex2d(line[j].x, line[j].y);
            //            cout << line[j] << endl;
        }
        glEnd();
//        ofEndShape();
    }

    
    float v = rms*100;
//    cout << v <<", " << ampv<< endl;
    float t = mouseY;
    if (v < mouseX) {
        ampv-=t;
    } else {
        ampv+=t;
    }
    ampv = MAX(0, ampv);
    
    
    float rad = 2;
//    if (gobig) {
//        rad = 10;
//        gobig = false;
//    }
	
    float q = ofMap(mouseX, 0, ofGetWidth(), 0, 1);
//    cout << rms << endl;
	int i = 0;
    for (vector<Vehicle*>::iterator it = points.begin(); it != points.end(); ++it) {
        
        (*it)->r = rad;
        (*it)->randomAmount = randAmount;

		if (gobig) {
			(*it)->followPath(points);
		}
		
        // Path following and separation are worked on in this function
        if (doCirclePoints) {


		(*it)->follow(paths[i%paths.size()]);
		i++;
		
		}
//        (*it)->follow();
//        (*it)->goToTarget();
        // Call the generic run method (update, borders, display, etc.)
//        (*it)->run();
        if(doMove) {
            (*it)->update();
			
			if (play) {
            (*it)->flock(points, ofMap(mouseX, 0, ofGetWidth(), 0, 3),
								 ofMap(mouseY, 0, ofGetHeight(), 0, 3));
			}
        }
        
        
//        (*it)->render();

		thing.draw((*it)->location, b, b); //mouseX, mouseX);
    }
    
//
////    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
////    ofScale(redSep, 0);
////    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
//    ofTranslate(0, -redSep);
//    for (vector<Vehicle*>::iterator it = points.begin(); it != points.end(); ++it) {
//        (*it)->r = 2;
//        if(doMove) (*it)->update();
//        (*it)->render(200, 0, 0);
//    }
	
    t+= 0.05;
    
//    fbo.end();
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//    glColor4f(1,1,1,1);
//    fbo.draw(0,0);

	cout << (ofGetElapsedTimef() - start) * 1000 << ",";
    
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