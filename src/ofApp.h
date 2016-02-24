#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxJSON.h"
#include "ofxAnimatableFloat.h"
#include "ofxAnimatableOfPoint.h"
#include "em/SceneCamera.h"
#include "em/SceneLight.h"
#include "em/MeshGenerator.h"
#include "em/Constants.h"


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

    void setupGui();

    void restoreParams();
    void saveParams(bool showDialog = false);
    
    void audioOut(ofSoundBuffer &outBuffer);
    
    inline void toggleAudio(bool &isEnabled){
        if (isEnabled) {
            // start the sound stream with a sample rate of 44100 Hz, and a buffer
            // size of 512 samples per audioOut() call
            ofSoundStreamSetup(2, 0, this->sampleRate, 512, 3);
            //        ofSoundStreamClose();
        } else {
            ofSoundStreamClose();
        }
    }
    
    inline void reset(){
        meshGenerator.clear();
//        sceneCam.setup(meshGenerator.getFixedParticlePosition(), "bg-light-gradient.png");
    }
    
    void audioIn(float * input, int bufferSize, int nChannels);
    ofSoundStream       soundStream;
    
    em::SceneCamera           sceneCam;
    ofImage                   bgImage;
    em::MeshGenerator         meshGenerator;
    vector<em::SceneLight>    lights;
    
    // Sound
    double sampleRate;
    double wavePhase;
    double pulsePhase;
    
    mutex audioMutex;
    ofSoundBuffer lastBuffer;
    ofPolyline waveform;
    float rms;
    ofParameter<bool>    audioEnabled;
    
    // Gui
    ofxPanel             gui;

    ofParameter<float>          fps;
    ofParameter<ofFloatColor>   globalAmbient;
    
    string settingsFileName;
    
    ofParameter<bool>   drawLights;
    ofParameter<bool>   drawGrid;
    ofParameter<bool>   drawWireframe;
    ofParameter<bool>   drawPolyMesh;
    ofParameter<bool>   drawSpringMesh;
    ofParameter<bool>   drawGui;
};
