#include "ofApp.h"


vector<ofVec3f> archimedianSphere(size_t samples, float radius, size_t numTurns=20) {
    
    int halfSamples = samples / 2;
    int skip = static_cast<int>(360.0f * numTurns / halfSamples);

    vector<ofVec3f> output;
    int j;
    float h, theta, fatness, x, y, z;
    
    for (int i = 0; i < halfSamples; i++) {
        j = i * skip;
        h = i / static_cast<float>(halfSamples / 2) - 1;
        theta = acos(h);
        fatness = sin(theta);
        y = cos(j * DEG_TO_RAD) * radius * fatness;
        x = h * radius;
        z = sin(j * DEG_TO_RAD) * radius * fatness;
        
        output.push_back(ofVec3f(x, y, z));
    }
    
    for (int i = halfSamples; --i > 0;) {
        j = i * skip;
        h = i / static_cast<float>(halfSamples / 2) - 1;
        theta = acos(h);
        fatness = sin(theta);
        y = cos(j * DEG_TO_RAD + PI) * radius * fatness;
        x = h * radius;
        z = sin(j * DEG_TO_RAD + PI) * radius * fatness;
        
        output.push_back(ofVec3f(x, y, z));
    }
    
    return output;
}

void ofApp::setup(){

	ofSetWindowShape(1280, 720);
    float offset = 30;
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB32F);

    thing.load("spark3.png");
	
    fbo.begin();
    ofClear(0,0,0,0);
    fbo.end();
    
	svg.load("/Users/whg/Desktop/TMRW logo hexagon black.svg");

    mFont.load("Arial.ttf", 100, true, true, true);
    
    for (int i = 0; i < 10000; i++) {
		flock.addAgent(ofVec3f(ofRandom(-ofGetWidth(), ofGetWidth()), ofRandom(-ofGetWidth(), ofGetHeight()), ofRandom(-ofGetWidth(), ofGetHeight())));
    }
	
    // Main Logo
	FollowPathCollection logo(svg);
    logo.centerPoints();
    flock.addPathCollection(std::move(logo));
    
    
    // Messages
    FollowPathCollection fontCollection;
    fontCollection.add(mFont.getStringAsPoints("Hello"));
    fontCollection.centerPoints();
    
    flock.addPathCollection(std::move(fontCollection));
    flock.addPathCollection(std::move(fontCollection)); // this for time...

    // IcoSphere
    ofIcoSpherePrimitive sphere(180, 1);
    const auto &verts = sphere.getMesh().getVertices();
    const auto &indices = sphere.getMesh().getIndices();

    vector<ofPath> paths;
    for (size_t i = 0; i < indices.size(); i+= 3) {
        ofPath path;
        path.moveTo(verts[indices[i]]);
        path.lineTo(verts[indices[i+1]]);
        path.lineTo(verts[indices[i+2]]);
        paths.push_back(path);
    }
    FollowPathCollection sphereCollection;
    sphereCollection.add(paths);
    flock.addPathCollection(std::move(sphereCollection));

    // Archimedian Spiral Sphere
    auto spiralVerts = archimedianSphere(flock.size(), 180);
    shared_ptr<FollowPath> spiralPath = make_shared<FollowPath>();
    spiralPath->addVertices(spiralVerts);
    FollowPathCollection spiralCollection;
    spiralCollection.add(spiralPath);
    flock.addPathCollection(std::move(spiralCollection));
	
	
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
    gui.add(mAlpha.set("alpha", 10, 0, 70));
    gui.add(mImageSize.set("image size", 2, 1, 30));
    gui.add(mSphereSize.set("sphere size", 1, 0, 1000));
    gui.add(mSphereIterations.set("sphere iterations", 1, 0, 4));
    
    mPathIndex.addListener(this, &ofApp::pathIndexChanged);
    gui.add(mPathIndex.set("path index", 0, 0, 4));


	p2lShader.load("points2lines.vert", "points2lines.frag", "points2lines.geom");

    
}

void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::update() {
    
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
}

//--------------------------------------------------------------
void ofApp::draw(){

    const auto &agents = flock.getAgents();
    
    fbo.begin();
    
    ofSetColor(0, 0, 0, mAlpha);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);

    ofMesh mesh;
    float hue = 0.4;
    for (const auto agent : agents) {
        
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
    
    
    p2lShader.setUniformMatrix4f("rotationMatrix", getCurrentRotationMatrix());
    
    mesh.setMode(OF_PRIMITIVE_LINES);
    mesh.draw(OF_MESH_FILL);

    
    p2lShader.end();

    fbo.end();

    fbo.draw(0, 0);

	gui.draw();

//    ofSetColor(255);
//    ofDrawBitmapString(ofToString(flock.getAgents().size()), 10, ofGetHeight() - 12);
	
}


void ofApp::keyPressed(int key) {
}

void ofApp::pathIndexChanged(int &index) {
    
    if (index == 2) {
        FollowPathCollection fontCollection;
        auto timeStamp = ofGetTimestampString("%H:%M");
        fontCollection.add(mFont.getStringAsPoints(timeStamp));
        fontCollection.centerPoints();
        flock.setPathCollection(2, std::move(fontCollection));
    }

    flock.assignAgentsToCollection(index, true);
}

ofMatrix4x4 ofApp::getCurrentRotationMatrix() {
    ofMatrix4x4 rotationMatrix;
    float t = ofGetElapsedTimef()*15;
    
    int r = static_cast<int>(t);
    float frac = t - r;
    r %= 360;
    float rot = r + frac;
    if (rot > 180) rot = 360 - rot;
    
    rotationMatrix.rotate(rot - 90, 0, 1, 0);
    return rotationMatrix;
}