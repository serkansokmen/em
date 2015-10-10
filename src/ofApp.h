#pragma once

#include "ofMain.h"
#include "MSAPhysics2D.h"
#include "ofxGui.h"

#define NODE_MIN_RADIUS     5
#define NODE_MAX_RADIUS     15

#define MIN_MASS            1
#define MAX_MASS            3

#define MIN_BOUNCE          0.01
#define MAX_BOUNCE          0.2

#define MIN_ATTRACTION      3
#define MAX_ATTRACTION      10

#define	SPRING_MIN_STRENGTH		0.005
#define SPRING_MAX_STRENGTH		0.1

#define	SPRING_MIN_WIDTH		1
#define SPRING_MAX_WIDTH		3

#define SECTOR_COUNT        1       // currently there is a bug at sector borders


using namespace msa::physics;

class Particle : public Particle2D {
public:
    ofColor color;
};


class ofApp : public ofBaseApp {
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void makeParticleAtPosition(float x, float y);
    void makeParticlesFromImage(ofImage &img);
    void setGravity(ofVec2f& g);
    void setupGui();
    
    World2D             physics;
    ofxPanel            gui;
    
    ofParameterGroup    particleParams;
    ofParameterGroup    springParams;
    
    ofParameter<ofVec2f> gravity;
    
    ofParameter<float>  node_min_radius;
    ofParameter<float>  node_max_radius;
    ofParameter<float>  min_mass;
    ofParameter<float>  max_mass;
    ofParameter<float>  min_bounce;
    ofParameter<float>  max_bounce;
    ofParameter<float>  min_attraction;
    ofParameter<float>  max_attraction;
    ofParameter<float>  spring_min_strength;
    ofParameter<float>  spring_max_strength;
    ofParameter<float>  spring_min_width;
    ofParameter<float>  spring_max_width;
};
