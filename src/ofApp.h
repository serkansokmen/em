#pragma once

#include "ofMain.h"
#include "MSAPhysics2D.h"
#include "MSAPhysics3D.h"
#include "ofxGui.h"
#include "ofxArcBall.h"

#define NODE_MIN_RADIUS     1
#define NODE_MAX_RADIUS     60

#define MIN_MASS            0.01
#define MAX_MASS            500

#define MIN_DISTANCE        0.1
#define MAX_DISTANCE        500

#define MIN_BOUNCE          0.0001
#define MAX_BOUNCE          0.95

#define MIN_ATTRACTION      -0.005
#define MAX_ATTRACTION      0.005

#define	SPRING_MIN_STRENGTH		0.00000005
#define SPRING_MAX_STRENGTH		0.000005
#define	SPRING_MIN_LENGTH		0.1
#define SPRING_MAX_LENGTH		400
#define SECTOR_COUNT            1

using namespace msa::physics;


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

    void makeParticleAtPosition(float x, float y, ofColor c);
    void makeParticlesFromImage(ofImage &img);

    template <typename T>
    void makeSpringBetweenParticles(ParticleT<T> *a, ParticleT<T> *b);

    void setGravity(ofPoint& g);
    void setupGui();

    World3D             physics;
    
    ofLight             pointLight;
    ofMaterial          polyMat, springMat;
    
    ofxPanel            gui;

    ofParameter<ofPoint> gravity;

    ofVboMesh            polygonMesh;
    ofVboMesh            springMesh;
    
    ofxArcBall           arcball;
    
    ofParameter<ofPoint> lightPos;
    ofParameter<float>   colorHue;
    ofColor              lightColor;
    ofColor              materialColor;
    ofParameter<bool>    physicsPaused;
    
    ofParameter<double>  node_radius;
    ofParameter<string>  springCount;
    ofParameter<double>  mass;
    ofParameter<double>  bounce;
    ofParameter<double>  attraction;
    ofParameter<double>  spring_strength;
    ofParameter<double>  spring_length;
    ofParameter<bool>    makeParticles;
    ofParameter<bool>    makeSprings;
    ofParameter<bool>    drawUsingVboMesh;
    ofParameter<bool>    drawGui;
    
    ofEasyCam   cam;
};
