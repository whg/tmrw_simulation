//
//  Path.h
//  emptyExample
//
//  Created by Will Gallia on 28/10/2014.
//
//

#pragma once

#include "ofMain.h"
namespace whg {

class Path : public ofPolyline {
public:
    float radius;
    
	Path() : radius(5) {
    }
};

}


