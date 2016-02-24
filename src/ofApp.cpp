#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofSetSmoothLighting(true);
    ofEnableArbTex();
    ofSetCircleResolution(100);

    float width = ofGetWidth();
    float height = ofGetHeight();
    
    for (int i=0; i<LIGHT_COUNT; i++){
        melt::SceneLight light;
        light.setup(i);
        lights.push_back(light);
    }
    
    
    setupGui();
    gui.minimizeAll();
    restoreParams();
    
    // Setup audio
    sampleRate = 44100;
    wavePhase = 0;
    pulsePhase = 0;
//    soundPlayer.load("08 Physics-Based Sound Synthesis for Games and Interactive Systems.mp3");
//    soundPlayer.play();
    
    meshGenerator.setup();
    sceneCam.setup(meshGenerator.getFixedParticlePosition(), "bg-light-gradient.png");
    soundStream.setup(this, 0, 2, 44100, 256, 4);
}

//-------------------------------------------------------------- 
void ofApp::setupGui(){
    
    settingsFileName = "settings.xml";
    
    float width = ofGetWindowWidth()/5;
    ofColor guiColor(0,0,0,255);
    
    gui.setup();
//    gui.setSize(width, ofGetHeight());
//    gui.setWidthElements(width);
//    gui.setDefaultWidth(width);
//    gui.setDefaultHeight(16);
    gui.add(fps.set("FPS", 0));
    gui.add(sceneCam.params);
    
    ofParameterGroup lightParams;
    lightParams.setName("Lights");
    lightParams.add(globalAmbient.set("Global ambient", ofFloatColor(.1,.1,.1), ofFloatColor(0,0,0), ofFloatColor(1,1,1)));
    for (auto & light : lights) {
        lightParams.add(light.params);
    }
    gui.add(lightParams);
    gui.add(meshGenerator.params);
    gui.add(drawPolyMesh.set("Draw polygon mesh", true));
    gui.add(drawSpringMesh.set("Draw spring mesh", true));
    gui.add(drawWireframe.set("Draw wireframe", false));
    gui.add(drawLights.set("Draw lights", true));
    gui.add(drawGrid.set("Draw grid", true));
    gui.add(drawGui.set("Keep settings open", true));
    gui.add(audioEnabled.set("Audio enabled", false));
    
    
    audioEnabled.addListener(this, &ofApp::toggleAudio);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetGlobalAmbientColor(globalAmbient);
    
    float bs = meshGenerator.boxSize / 2;
    meshGenerator.update();
    
    // Update audio
    unique_lock<mutex> lock(audioMutex);
    waveform.clear();
    for(size_t i = 0; i < lastBuffer.getNumFrames(); i++) {
        float sample = lastBuffer.getSample(i, 0);
        float x = ofMap(i, 0, lastBuffer.getNumFrames(), 0, ofGetWidth());
        float y = ofMap(sample, -1, 1, 0, ofGetHeight());
        waveform.addVertex(x, y);
    }
    rms = lastBuffer.getRMSAmplitude();
    
    for (auto & light : lights) {
        light.update(bs);
    }
    
    fps.set(ofGetFrameRate());
    
    ofSetColor(ofColor::white);
    ofSetLineWidth(1 + (rms * 30.));
    waveform.draw();
}

//--------------------------------------------------------------
void ofApp::audioIn(float *input, int bufferSize, int nChannels){
    if (sceneCam.isRecording()){
        sceneCam.addAudioSamplesToRecording(input, bufferSize, nChannels);
    }
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer){
    // base frequency of the lowest sine wave in cycles per second (hertz)
    float frequency = 172.5;
    
    // mapping frequencies from Hz into full oscillations of sin() (two pi)
    float wavePhaseStep = (frequency / sampleRate) * TWO_PI;
    float pulsePhaseStep = (0.5 / sampleRate) * TWO_PI;
    
    // this loop builds a buffer of audio containing 3 sine waves at different
    // frequencies, and pulses the volume of each sine wave individually. In
    // other words, 3 oscillators and 3 LFOs.
    
    for(size_t i = 0; i < outBuffer.getNumFrames(); i++) {
        
        // build up a chord out of sine waves at 3 different frequencies
        float sampleLow = sin(wavePhase);
        float sampleMid = sin(wavePhase * 1.5);
        float sampleHi = sin(wavePhase * 2.0);
        
        // pulse each sample's volume
        sampleLow *= sin(pulsePhase);
        sampleMid *= sin(pulsePhase * 1.04);
        sampleHi *= sin(pulsePhase * 1.09);
        
        float fullSample = (sampleLow + sampleMid + sampleHi);
        
        // reduce the full sample's volume so it doesn't exceed 1
        fullSample *= 0.3;
        
        // write the computed sample to the left and right channels
        outBuffer.getSample(i, 0) = fullSample;
        outBuffer.getSample(i, 1) = fullSample;
        
        // get the two phase variables ready for the next sample
        wavePhase += wavePhaseStep;
        pulsePhase += pulsePhaseStep;
    }
    
    unique_lock<mutex> lock(audioMutex);
    lastBuffer = outBuffer;
}

//--------------------------------------------------------------
void ofApp::restoreParams(){
    gui.loadFromFile(settingsFileName);
}

//--------------------------------------------------------------
void ofApp::saveParams(bool showDialog){
    if (showDialog) {
        ofFileDialogResult res;
        res = ofSystemSaveDialog(settingsFileName, "Save params");
        if (res.bSuccess) {
            gui.saveToFile(res.filePath);
            settingsFileName = res.fileName;
        }
    } else {
        gui.saveToFile(settingsFileName);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    sceneCam.begin();
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableLighting();
    if (drawLights) {
        for (auto & light : lights) {
            light.draw();
        }
    }
    if (drawGrid) {
        ofSetColor(255, 10);
        float stepSize = meshGenerator.boxSize/4;
        size_t numberOfSteps = 4;
        bool labels = false;
        ofDrawGrid(stepSize, numberOfSteps, labels);
    }
    meshGenerator.draw(drawPolyMesh, drawSpringMesh, drawWireframe);
    ofDisableDepthTest();
    ofDisableAlphaBlending();
    ofDisableLighting();
    sceneCam.end();
    
    
    sceneCam.draw((ofGetWidth()-FBO_WIDTH/2)/2,
                  (ofGetHeight()-FBO_HEIGHT/2)/2,
                  FBO_WIDTH/2, FBO_HEIGHT/2);
    
    if (drawGui) {
        ofEnableAlphaBlending();
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {

        case 'c':
            meshGenerator.makeCluster();
            break;
        case 'o':
            sceneCam.orbitCamera = !sceneCam.orbitCamera;
            break;
            
        case 's':
            saveParams();
            break;
        case 'S':
            saveParams(true);
            break;
        case 'l':
            restoreParams();
            break;
        case 'F':
            ofToggleFullscreen();
            break;
        case 'g':
            drawGrid = !drawGrid;
            break;
        case ',':
            drawGui = !drawGui;
        case 'm':
            gui.minimizeAll();
            break;
        case 'n':
            gui.maximizeAll();
            break;
        
        case ' ':
            reset();
            break;
            
        case '1':
            meshGenerator.makeParticles = !meshGenerator.makeParticles;
            break;
        case '2':
            meshGenerator.makeSprings = !meshGenerator.makeSprings;
            break;
        
        case '.':
            meshGenerator.randomiseParams();
            break;
        case 'Q':
            meshGenerator.saveMesh(true, true);
            break;
        
        case 'r':
            sceneCam.toggleRecording();
            break;
        case 'R':
            sceneCam.endRecording();
            break;
            
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == 0) {
//        int n = physics.numberOfParticles();
//        float nearestDistance = 0;
//        ofVec2f nearestVertex;
//        int nearestIndex = 0;
//        ofVec2f mouse(x, y);
//        for(int i = 0; i < n; i++) {
//            ofVec3f cur = previewCam.worldToScreen(physics.getParticle(i)->getPosition());
//            float distance = cur.distance(mouse);
//            if(i == 0 || distance < nearestDistance) {
//                nearestDistance = distance;
//                nearestVertex = cur;
//                nearestIndex = i;
//            }
//        }
//        ofPoint pos = physics.getParticle(nearestIndex)->getPosition();
//        previewCam.lookAt(pos);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
//    physics.clearWorldSize();
//    physics.setWorldSize(ofPoint(-w, -h, -h), ofPoint(w, h, h));
//    gui.setWidthElements(w/5);
//    gui.setDefaultWidth(w/5);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo info){
}
