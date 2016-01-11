#pragma once

#include "ofMain.h"
#include "MSAPhysics3D.h"
#include "ofxGui.h"
#include "ofxJSON.h"
#include "ofxCameraSaveLoad.h"
#include "ofxAnimatableOfPoint.h"
#include "ofxMeshUtils.h"
#include "Leap.h"

// design a boxed thing in a way that it can interpret data in a visual way


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


namespace gcv {
    
    struct NodeType {
        string      id;
        string      name;
        string      description;
        string      image;
        ofColor     color;
    };
    struct EdgeType {
        string      id;
        string      name;
        string      description;
        double      weighted;
        double      directed;
        ofColor     color;
    };
    
    struct Node {
        string      id;
        string      name;
        string      description;
        string      image;
        string      type_id;
        ofColor     color;
        double      pos_x;
        double      pos_y;
    };
    struct Edge {
        string      id;
        string      name;
        string      from_node_id;
        string      to_node_id;
        string      type_id;
        double      weight;
        bool        directed;
    };
    class Particle : public Particle3D {
    public:
        Node    node;
        ofColor color;
    };
};



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
    
    void loadJson(const string& url);

    template <typename T>
    void makeSpringBetweenParticles(ParticleT<T> *a, ParticleT<T> *b);

    // Event Handlers
    void toggleLeap(bool& v);
    void setPhysicsBoxSize(double& s);
    void setupShading();
    void setupGui();
    void randomiseParams();

    void restoreParams();
    void saveParams(bool showDialog = false);
    
    
    
    inline void setGravityVec(ofPoint& g){
        physics.setGravity(g);
    }
    inline void setCamFov(float& v) {
        previewCam.setFov(v);
    }
    inline void setZDepth(float& v) {
        for (int i=0; i<physics.numberOfParticles(); i++) {
            auto p = physics.getParticle(i);
            ofPoint pos(p->getPosition());
            pos.z = ofRandom(-v, v);
            p->moveTo(pos);
        }
    }
    inline void setCamNearClip(float& v) {
        previewCam.setNearClip(v);
    }
    inline void setCamFarClip(float& v) {
        previewCam.setFarClip(v);
    }
    inline void setFixedParticleMoveDuration(float& val) {
        fixedParticlePos.setDuration(val);
    };
    inline int getHexFromColorName(string colorName){
        return ofHexToInt(colorName.replace(0, 1, "0x"));
    };
    

    World3D              physics;
    ofEasyCam            previewCam;
    ofLight              pLight0, pLight1;
    vector<ofLight>      lights;

    ofMaterial           polyMat, springMat;
    ofShader             shader;

    Leap::Controller     leap;
    ofxPanel             gui;
    
    ofxJSONElement          json;
    vector<gcv::NodeType>   nodeTypes;
    vector<gcv::EdgeType>   edgeTypes;
    vector<gcv::Node>       nodes;
    vector<gcv::Edge>       edges;
    vector<gcv::Particle*>  particles;

    of3dPrimitive        polyPrimitive;
    of3dPrimitive        springPrimitive;
    ofImage              polyTextureImage;
    ofVboMesh            polyMesh, springMesh, nodeMesh;
    ofShader             polyShader, springShader;
    Particle3D           fixedParticle;
    ofxAnimatableOfPoint fixedParticlePos;
    ofParameter<float>   fixedParticleMoveDuration;

    ofQuaternion         camQuat;
    ofPoint              camPos;
    
//    Physics params
    ofParameter<bool>    makeParticles, makeSprings;
    ofParameter<double>  radius;
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
    ofParameter<bool>    enableLight0, enableLight1;
    ofParameter<bool>    drawWireframe;
    ofParameter<bool>    drawUsingVboMesh;
    ofParameter<bool>    orbitCamera;
    ofParameter<bool>    orbitLight0, orbitLight1;
    ofParameter<bool>    drawLights;
    ofParameter<bool>    drawSprings;
    ofParameter<bool>    drawLabels;

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
    string currentGraphName;


    ofParameter<bool>    useLeap;
    ofParameter<bool>    drawGrid;
    ofParameter<bool>    drawGui;
};
