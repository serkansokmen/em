#pragma once

#include "ofMain.h"
#include "MSAPhysics3D.h"
#include "ofxGui.h"
#include "ofxJSON.h"
#include "ofxCameraSaveLoad.h"
#include "ofxAnimatableFloat.h"
#include "ofxAnimatableOfPoint.h"
#include "ofxVideoRecorder.h"
#include "ofxAssimpModelLoader.h"

#define FBO_WIDTH       1920
#define FBO_HEIGHT      1080

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

#define MAX_PARTICLES       2000

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
    void renderScene();
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
    void setPhysicsBoxSize(double& s);
    void setupShading();
    void setupGui();
    void randomiseParams();

    void restoreParams();
    void saveParams(bool showDialog = false);
    
    void audioOut(ofSoundBuffer &outBuffer);
    
    inline void toggleAnimBoxSize(bool& val) {
        if (val && !boxSizeAnimate.isAnimating()) {
            boxSizeAnimate.setDuration(boxSize/100.f);
            boxSizeAnimate.animateFromTo(boxSize, boxSize*3.2);
        } else {
            boxSizeAnimate.reset();
        }
        
    };
    inline void setGravityVec(ofPoint& g){
        physics.setGravity(g);
    };
    inline void setCamFov(float& v) {
        previewCam.setFov(v);
    };
    inline void setZDepth(float& v) {
        for (int i=0; i<physics.numberOfParticles(); i++) {
            auto p = physics.getParticle(i);
            ofPoint pos(p->getPosition());
            pos.z = ofRandom(-v, v);
            p->moveTo(pos);
        }
    };
    inline void setCamNearClip(float& v) {
        previewCam.setNearClip(v);
    };
    inline void setCamFarClip(float& v) {
        previewCam.setFarClip(v);
    };
    inline void setFixedParticleMoveDuration(float& val) {
        fixedParticlePos.setDuration(val);
    };
    inline int getHexFromColorName(string colorName){
        return ofHexToInt(colorName.replace(0, 1, "0x"));
    };
    
    void audioIn(float * input, int bufferSize, int nChannels);
    ofSoundStream       soundStream;
    
    ofxVideoRecorder     vidRecorder;
    ofFbo                screenFbo;
    void setupScreenFbo();
    string fileName;
    string fileExt;
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
    ofFbo recordFbo;
    ofPixels recordPixels;
    bool bRecording;
    
    ofxAssimpModelLoader modelLoader;
    vector<ofVec3f> offsets;
    
    World3D              physics;
    ofEasyCam            previewCam;
    ofLight              pLight0, pLight1;
    vector<ofLight>      lights;
    ofxAnimatableFloat   boxSizeAnimate;

    ofMaterial           polyMat, springMat;
    ofShader             shader;
    
    // Sound
    double sampleRate;
    double wavePhase;
    double pulsePhase;
    
    mutex audioMutex;
    ofSoundBuffer lastBuffer;
    ofPolyline waveform;
    float rms;
    ofParameter<bool>    audioEnabled;
    void toggleAudio(bool& isEnabled);
    
    // Gui
    ofxPanel             gui;

    of3dPrimitive        polyPrimitive;
    of3dPrimitive        springPrimitive;
    ofVboMesh            polyMesh, springMesh;
    ofShader             polyShader, springShader;
    Particle3D           fixedParticle;
    ofxAnimatableOfPoint fixedParticlePos;
    ofParameter<float>   fixedParticleMoveDuration;

    ofQuaternion         camQuat;
    ofPoint              camPos;
    
    ofImage              bgImage;
    
//    Physics params
    ofParameter<bool>    makeParticles, makeSprings;
    ofParameter<double>  radius;
    ofParameter<double>  drag;
    ofParameter<double>  mass;
    ofParameter<double>  bounce;
    ofParameter<double>  attraction;
    ofParameter<double>  springStrength;
    ofParameter<double>  springLength;
    ofParameter<double>  boxSize;
    ofParameter<float>   zDepth;
    ofParameter<int>     particleCount;
    ofParameter<int>     springCount;
    ofParameter<int>     attractionCount;
    ofParameter<float>   fps;
    ofParameter<ofPoint> gravity;
    ofParameter<bool>    bindToFixedParticle;
    ofParameter<bool>    moveOnClick;
    ofParameter<bool>    physicsPaused;

//    Camera params
    ofParameter<float>   camFov;
    ofParameter<float>   camNearClip;
    ofParameter<float>   camFarClip;

//     Render params
    ofParameter<float>   lightOrbitSpeed;
    ofParameter<double>  lightOrbitRadius;
    ofParameter<bool>    enableLight0, enableLight1;
    ofParameter<bool>    drawWireframe;
    ofParameter<bool>    drawPolyMesh,
                         drawSpringMesh,
                         drawLights,
                         doNodeShader;
    ofParameter<bool>    orbitCamera;
    ofParameter<bool>    orbitLight0, orbitLight1;

//      Material params
    ofParameter<ofFloatColor>   lightAmbient0, lightDiffuse0, lightSpecular0,
                                lightAmbient1, lightDiffuse1, lightSpecular1;
    ofParameter<ofFloatColor>   polygonAmbient, polygonDiffuse, polygonSpecular;
    ofParameter<ofFloatColor>   springAmbient, springDiffuse, springSpecular;
    ofParameter<ofFloatColor>   globalAmbient;
    ofParameter<float>          polygonShininess, springShininess;
    ofParameter<float>          lightAttenuation;
    ofParameter<float>          attConstant0, attLinear0, attQuadratic0,
                                attConstant1, attLinear1, attQuadratic1;
    
    string settingsFileName;
    string currentGraphName;

    ofParameter<bool>    drawGrid;
    ofParameter<bool>    drawGui;
    ofParameter<bool>    animateBoxSize;
};
