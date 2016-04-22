//
//  Vehicle.h
//  emptyExample
//
//  Created by Will Gallia on 28/10/2014.
//
//

#pragma once


#include "ofMain.h"
using namespace whg;
class Vehicle {
public:

    // All the usual stuff
    ofVec3f location;
    ofVec3f velocity;
    ofVec3f acceleration;
    float r;
    float maxforce;    // Maximum steering force
    float maxspeed;    // Maximum speed
    
    ofVec3f target;
    float randomAmount;
    ofVec3f random;
    bool foundTarget;
    
    int targetIndex, currentPointIndex;
    
    whg::Path path;
    
    bool debug = true;

    
    Vehicle() {
        int w2 = ofGetWidth() >> 1;
        int h2 = ofGetHeight() >> 1;
        location = ofVec3f(ofRandom(-w2, w2), ofRandom(-h2, h2));
//        location = ofVec3f(0, 0);
        r = 2;
        maxspeed = 1; //ofRandom(2, 4);
        maxforce = 0.1;
        acceleration = ofVec3f(0, 0);
        velocity = ofVec3f(ofRandom(maxspeed), ofRandom(maxspeed));
        randomAmount = 0;
        random = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1));
        random.normalize();
        foundTarget = false;
        targetIndex = 0;

    }
    
    // Constructor initialize all values
    Vehicle(ofVec3f l, float ms, float mf) {
        location = ofVec3f(l);
        r = 12;
        maxspeed = ms;
        maxforce = mf;
        acceleration = ofVec3f(0, 0);
        velocity = ofVec3f(maxspeed, 0);
    }
    
    // A function to deal with path following and separation
    void applyBehaviors(vector<Vehicle> &vehicles) {
        // Follow path force
//        ofVec3f f = follow(path);
        
        ofVec3f f = follow();
        
        
        // Separate from other boids force
//        ofVec3f s = separate(vehicles);
        // Arbitrary weighting
//        f *= 3.0;
        //    s.mult(1);
        // Accumulate in acceleration
        applyForce(f);
//        applyForce(s);
    }
    
    void followPath(vector<Vehicle*> &vehicles, float amount=3.0) {
        // Follow path force
//        ofVec3f f = follow(path);
        ofVec3f f = moveAlong();
        f *= amount;
        applyForce(f);
    }
    
    void applyForce(ofVec3f force) {
        // We could add mass here if we want A = F / M
        acceleration += force;
    }
    
    
    
    // Main "run" function
    void run() {
//        update();
        borders();
        render();
    }
    
    void goToTarget() {
        
        
        ofVec3f target = path.getVertices()[currentPointIndex];
        ofVec3f dir = target - location;
        dir.normalize();
        dir*= maxspeed;
        cout << target.distance(location) << endl;
        if (target.distance(location) < 2) {
            foundTarget = true;
        }
        ofSetColor(255, 255, 0);
        ofCircle(target.x, target.y, 10);
        applyForce(dir);
    }
    
    ofVec3f follow() {
        
        ofVec3f predict = ofVec3f(velocity);
        predict.normalize();
        predict *= 25.0;
        ofVec3f predictLoc = location + predict;
    

        // Get the normal point to that line
        ofVec3f normal = getNormalPoint(predictLoc, location, target);
        ofVec3f dir = target - location;
        
//        ofRect(target.x, target.y, 5, 5);
        if (true) {
            // Draw predicted future location
            ofNoFill();
            ofSetColor(255, 0, 0);
            ofLine(location.x, location.y, predictLoc.x, predictLoc.y);
            ofEllipse(predictLoc.x, predictLoc.y, 4, 4);
            
            // Draw normal location
            
            ofSetColor(0, 255, 0);
            ofEllipse(normal.x, normal.y, 4, 4);
            // Draw actual target (red if steering towards it)
            ofLine(predictLoc.x, predictLoc.y, target.x, target.y);
//            if (worldRecord > p.radius) ofSetColor(255, 0, 255);
            ofFill();
            ofEllipse(target.x, target.y, 8, 8);
        }

        float d = predictLoc.distance(target);
        if (d < 5) {
            cout << "early" << endl;
//            velocity.x = 0; velocity.y = 0;
            return ofVec3f(0, 0);
        }
        
        // Look at the direction of the line segment so we can seek a little bit ahead of the normal
        dir.normalize();
        // This is an oversimplification
        // Should be based on distance to path & velocity
        
        dir*= 25;
        applyForce((normal + dir) * 3.0);
        return normal + dir;
    }
    
    ofVec3f moveAlong() {
        
        const vector<ofVec3f> points = path.getVertices();

        ofVec3f target = points[currentPointIndex];
//        if (!foundTarget) {
            if (target.distance(location) < 5) {
                foundTarget = true;
                currentPointIndex++;
                currentPointIndex %= points.size();
            }
//            else {
                return seek(target);
//            }

//        }
        
        
//        cout << points[currentPointIndex].distance(location) << endl;
        
        if (points[currentPointIndex].distance(location) < 5) {
            currentPointIndex++;
            currentPointIndex %= points.size();
        }
        
        // Predict location 25 (arbitrary choice) frames ahead
        ofVec3f predict = ofVec3f(velocity);
        predict.normalize();
        predict *= 25.0;
        ofVec3f predictLoc = location + predict;
        
        // Now we must find the normal to the path from the predicted location
        // We look at the normal for each line segment and pick out the closest one
        ofVec3f normal;
//        ofVec3f target;
        float worldRecord = 1000000;  // Start with a very high worldRecord distance that can easily be beaten
        
        
        //        for(int i = 0; i < )
        
        // Loop through all points of the path
//        for (int i = 0; i < points.size(); i++) {
        
            // Look at a line segment
        ofVec3f a = points[currentPointIndex];
        ofVec3f b = points[(currentPointIndex+1)%points.size()]; // Note Path has to wraparound
//        cout << currentPointIndex << endl;
        
        if (debug) {
            ofSetColor(255, 0, 0);
            ofEllipse(a.x, a.y, 5, 5);
            ofSetColor(255, 0, 255);
            ofEllipse(b.x, b.y, 5, 5);
        }
        
        // Get the normal point to that line
        ofVec3f normalPoint = getNormalPoint(predictLoc, a, b);
        
        // Check if normal is on line segment
        ofVec3f dir = b - a;
        // If it's not within the line segment, consider the normal to just be the end of the line segment (point b)
        //if (da + db > line.mag()+1) {
//        if (normalPoint.x < min(a.x,b.x) || normalPoint.x > max(a.x,b.x) || normalPoint.y < min(a.y,b.y) || normalPoint.y > max(a.y,b.y)) {
//            normalPoint = ofVec3f(b);
//            // If we're at the end we really want the next line segment for looking ahead
//            a = points[(i+1)%points.size()];
//            b = points[(i+2)%points.size()];  // Path wraps around
//            dir = b - a;
//        }
        
        // How far away are we from the path?
        float d = predictLoc.distance(normalPoint);
        // Did we beat the worldRecord and find the closest line segment?
        if (true || d < worldRecord) {
            worldRecord = d;
            normal = normalPoint;
            
            // Look at the direction of the line segment so we can seek a little bit ahead of the normal
            dir.normalize();
            // This is an oversimplification
            // Should be based on distance to path & velocity
            
            dir*= 25;
            target = ofVec3f(normal);
            target+= dir;
            
        }
//        }
        
        
        if (debug) {
            // Draw predicted future location
            ofNoFill();
            ofSetColor(255, 0, 0);
            ofLine(location.x, location.y, predictLoc.x, predictLoc.y);
            ofEllipse(predictLoc.x, predictLoc.y, 4, 4);
            
            // Draw normal location
            
            ofSetColor(0, 255, 0);
            ofEllipse(normal.x, normal.y, 4, 4);
            // Draw actual target (red if steering towards it)
            ofLine(predictLoc.x, predictLoc.y, target.x, target.y);
            if (worldRecord > path.radius) ofSetColor(255, 0, 255);
            ofFill();
            ofEllipse(target.x, target.y, 8, 8);
        }
        
        
        // Only if the distance is greater than the path's radius do we bother to steer
        if (worldRecord > path.radius) {
            return seek(target);
        }
        else {
            return ofVec3f(0, 0);
        }
    }
    
    
    // This function implements Craig Reynolds' path following algorithm
    // http://www.red3d.com/cwr/steer/PathFollow.html
    ofVec3f follow(const whg::Path &p) {
        
      
        
        // Predict location 25 (arbitrary choice) frames ahead
        ofVec3f predict = ofVec3f(velocity);
        predict.normalize();
        predict *= 5.0;
        ofVec3f predictLoc = location + predict;
        
        // Now we must find the normal to the path from the predicted location
        // We look at the normal for each line segment and pick out the closest one
        ofVec3f normal;
        ofVec3f target;
        float worldRecord = 1000000;  // Start with a very high worldRecord distance that can easily be beaten
        
        const vector<ofVec3f> points = p.getVertices();
        
//        for(int i = 0; i < )
        
        // Loop through all points of the path
        for (int i = 0; i < points.size(); i++) {
            
            // Look at a line segment
            ofVec3f a = points[i];
            ofVec3f b = points[(i+1)%points.size()]; // Note Path has to wraparound
            
            // Get the normal point to that line
            ofVec3f normalPoint = getNormalPoint(predictLoc, a, b);
            
            // Check if normal is on line segment
            ofVec3f dir = b - a;
            // If it's not within the line segment, consider the normal to just be the end of the line segment (point b)
            //if (da + db > line.mag()+1) {
            if (normalPoint.x < min(a.x,b.x) || normalPoint.x > max(a.x,b.x) || normalPoint.y < min(a.y,b.y) || normalPoint.y > max(a.y,b.y)) {
                normalPoint = ofVec3f(b);
                // If we're at the end we really want the next line segment for looking ahead
                a = points[(i+1)%points.size()];
                b = points[(i+2)%points.size()];  // Path wraps around
                dir = b - a;
            }
            
            // How far away are we from the path?
            float d = predictLoc.distance(normalPoint);
            // Did we beat the worldRecord and find the closest line segment?
            if (d < worldRecord) {
                worldRecord = d;
                normal = normalPoint;
                
                // Look at the direction of the line segment so we can seek a little bit ahead of the normal
                dir.normalize();
                // This is an oversimplification
                // Should be based on distance to path & velocity

                dir*= 25;
                target = ofVec3f(normal);
                target+= dir;
                
            }
        }
        
        
        if (false) {
            // Draw predicted future location
            ofNoFill();
            ofSetColor(255, 0, 0);
//            ofLine(location.x, location.y, predictLoc.x, predictLoc.y);
//            ofEllipse(predictLoc.x, predictLoc.y, 4, 4);
//            
//            // Draw normal location
//            
//            ofSetColor(0, 255, 0);
//            ofEllipse(normal.x, normal.y, 4, 4);
//            // Draw actual target (red if steering towards it)
//            ofLine(predictLoc.x, predictLoc.y, target.x, target.y);
//            if (worldRecord > p.radius) ofSetColor(255, 0, 255);
//            ofFill();
            ofEllipse(target.x, target.y, 8, 8);
        }
//		cout << worldRecord << endl;
		
        // Only if the distance is greater than the path's radius do we bother to steer
        if (worldRecord > 0) { //p.radius) {
			ofVec3f v = seek(target);
			applyForce(v);
			return v;
        }
        else {
            return ofVec3f(0, 0);
        }
    }
    
    
    // A function to get the normal point from a point (p) to a line segment (a-b)
    // This function could be optimized to make fewer new Vector objects
    ofVec3f getNormalPoint(ofVec3f p, ofVec3f a, ofVec3f b) {
        // Vector from a to p
        ofVec3f ap = p - a;
        // Vector from a to b
        ofVec3f ab = b - a;
        ab.normalize(); // Normalize the line
        // Project vector "diff" onto line by using the dot product
        ab *= ap.dot(ab);
        ofVec3f normalPoint = a + ab;
        return normalPoint;
    }
    
    // Separation
    // Method checks for nearby boids and steers away
    ofVec3f separate (vector<Vehicle*> &boids) {
		float desiredseparation = 20; //r*2;
        ofVec3f steer = ofVec3f();
        int count = 0;
        // For every boid in the system, check if it's too close
        for (int i = 0 ; i < boids.size(); i++) {
            Vehicle *other = boids[i];
//            float d = ofVec3f.dist(location, other.location);
            float d = location.distance(other->location);
            // If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
            if ((d > 0) && (d < desiredseparation)) {
                // Calculate vector pointing away from neighbor
//                ofVec3f diff = ofVec3f.sub(location, other.location);
                ofVec3f diff = location - other->location;
                diff.normalize();
                diff/= d;        // Weight by distance
                steer+= diff;
                count++;            // Keep track of how many
            }
        }
        // Average -- divide by how many
        if (count > 0) {
            steer/= (float) count;
        }
        
        // As long as the vector is greater than 0
        if (steer.length() > 0) {
            // Implement Reynolds: Steering = Desired - Velocity
            steer.normalize();
            steer*= maxspeed;
            steer-= velocity;
            steer.limit(maxforce);
        }
        return steer;
    }
    
    ofVec3f cohesion (vector<Vehicle*> &boids) {
        float neighbordist = 200;
        ofVec3f sum(0, 0);
        int count = 0;
        for (Vehicle *other : boids) {
            float d = (location - other->location).length();
            if ((d > 0) && (d < neighbordist)) {
                sum+= other->location; // Add location
                count++;
            }
        }
        if (count > 0) {
            sum/= count;
            return seek(sum);  // Steer towards the location
        } 
        else {
            return ofVec3f(0, 0);
        }
    }
    
    
    // Method to update location
    void update() {
        // Update velocity
        velocity+= acceleration;
        // Limit speed
        velocity.limit(maxspeed);
        location+= velocity;
        // Reset accelertion to 0 each cycle
        acceleration*= 0;
        
        borders();
    }
    
    void flock(vector<Vehicle*> &boids, float sepAmount=0, float cohAmount=0) {
        ofVec3f sep = separate(boids);   // Separation
        ofVec3f coh = cohesion(boids);   // Cohesion
        // Arbitrarily weight these forces
        sep*= sepAmount;
        coh*= cohAmount;
        // Add the force vectors to acceleration
        applyForce(sep);
        applyForce(coh);
    }
    
    // A method that calculates and applies a steering force towards a target
    // STEER = DESIRED MINUS VELOCITY
    ofVec3f seek(ofVec3f target) {
        ofVec3f desired = target - location;  // A vector pointing from the location to the target
        
        // Normalize desired and scale to maximum speed
        desired.normalize();
        desired *= maxspeed;

        // Steering = Desired minus Velocationity
        ofVec3f steer = desired - velocity;
        steer.limit(maxforce);  // Limit to maximum steering force
        
        return steer;
    }
    
    
    
    void render(int rc=255, int gc=255, int bc=255) {
        // Simpler boid is just a circle
        
        ofSetColor(rc, gc, bc, 130);
        ofPushMatrix();
		ofVec3f p = location;// + random * randomAmount;
        ofTranslate(p.x, p.y);
        ofFill();
        ofEllipse(0, 0, r, r);
        ofPopMatrix();
    }
    
    // Wraparound
    void borders() {
        if (location.x < -r) location.x = ofGetWidth() + r;
        if (location.y < -r) location.y = ofGetHeight()+r;
        if (location.x > ofGetWidth() + r) location.x = -r;
        if (location.y >  ofGetHeight()+r) location.y = -r;
    }
};



