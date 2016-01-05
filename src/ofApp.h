#pragma once

#include "ofMain.h"
#include "MSAPhysics2D.h"
#include "MSAPhysics3D.h"
#include "ofxGui.h"
#include "ofxCameraSaveLoad.h"
#include "Leap.h"


#define PARTICLE_MIN_RADIUS 0.5
#define PARTICLE_MAX_RADIUS 200

#define MIN_MASS            0.01
#define MAX_MASS            1

#define MIN_DISTANCE        0.1
#define MAX_DISTANCE        500

#define MIN_BOUNCE          0.0
#define MAX_BOUNCE          0.95

#define MIN_ATTRACTION      0.0
#define MAX_ATTRACTION      10.0

#define	SPRING_MIN_STRENGTH		0.00005
#define SPRING_MAX_STRENGTH		0.05
#define	SPRING_MIN_LENGTH		0.1
#define SPRING_MAX_LENGTH		1200
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

    void makeParticleAtPosition(const ofPoint& p);
    void makeParticleAtCenter(float radius);
    void makeCluster();

    template <typename T>
    void makeSpringBetweenParticles(ParticleT<T> *a, ParticleT<T> *b);

    // Event Handlers
    void toggleLeap(bool& v);
    void setPhysicsBoxSize(double& s);
    void setGravityVec(ofPoint& g);
    void setCamFov(float& v);
    void setCamNearClip(float& v);
    void setCamFarClip(float& v);
    void setupShading();
    void setupGui();
    void resetCamera();
    void randomiseParams();

    void restoreParams();
    void saveParams(bool showDialog = false);

    World3D              physics;
    ofEasyCam            previewCam;
    ofLight              pLight0, pLight1;

    ofMaterial           polyMat, springMat;

    Leap::Controller     leap;

    ofxPanel             gui;

    of3dPrimitive        polyPrimitive;
    of3dPrimitive        springPrimitive;
    ofImage              polyTextureImage;
    ofVboMesh            polyMesh;
    ofVboMesh            springMesh;
    Particle3D           fixedParticle;

    ofQuaternion         camQuat;
    ofPoint              camPos;

//    Physics params
    ofParameter<bool>    makeParticles, makeSprings;
    ofParameter<double>  radius;
    ofParameter<double>  mass;
    ofParameter<double>  bounce;
    ofParameter<double>  attraction;
    ofParameter<double>  spring_strength;
    ofParameter<double>  spring_length;
    ofParameter<double>  boxSize;
    ofParameter<string>  particleCount;
    ofParameter<string>  springCount;
    ofParameter<string>  attractionCount;
    ofParameter<ofPoint> gravity;
    ofParameter<bool>    bindToFixedParticle;
    ofParameter<bool>    physicsPaused;

//    Camera params
    ofParameter<float>   camFov;
    ofParameter<float>   camNearClip;
    ofParameter<float>   camFarClip;

//     Render params
    ofParameter<bool>    enableLight0, enableLight1;
    ofParameter<bool>    drawWireframe;
    ofParameter<bool>    drawUsingVboMesh;
    ofParameter<bool>    orbitCamera;
    ofParameter<bool>    orbitLight0, orbitLight1;
    ofParameter<bool>    drawLights;
    ofParameter<bool>    drawSprings;

//      Material params
    ofParameter<ofFloatColor>   lightAmbient0, lightDiffuse0, lightSpecular0,
                                lightAmbient1, lightDiffuse1, lightSpecular1;
    ofParameter<ofFloatColor>   polygonAmbient, polygonDiffuse, polygonSpecular;
    ofParameter<ofFloatColor>   springAmbient, springDiffuse, springSpecular;
    ofParameter<float>          polygonShininess, springShininess;
    ofParameter<float>          lightAttenuation;
    ofParameter<float>          attConstant0, attLinear0, attQuadratic0,
                                attConstant1, attLinear1, attQuadratic1;
    
    string settingsFileName;


    ofParameter<bool>    useLeap;
    ofParameter<bool>    drawGrid;
    ofParameter<bool>    drawGui;
};
