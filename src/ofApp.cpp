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

    setupShading();
    setupGui();
    gui.minimizeAll();
    restoreParams();
    
    fixedParticlePos.setPosition(ofPoint(0,0,0));
    fixedParticlePos.setRepeatType(PLAY_ONCE);
    fixedParticlePos.setCurve(EXPONENTIAL_SIGMOID_PARAM);
    
    boxSizeAnimate.setRepeatType(LOOP_BACK_AND_FORTH);
    boxSizeAnimate.setCurve(LATE_EASE_IN_EASE_OUT);
    boxSizeAnimate.setDuration(lightOrbitSpeed*4);
    
    // Setup audio
    sampleRate = 44100;
    wavePhase = 0;
    pulsePhase = 0;
    
    
    
//    fixedParticlePos.setRepeatTimes(3);
    //pointAnim.setAutoFlipCurve(true);
    
//    soundPlayer.load("08 Physics-Based Sound Synthesis for Games and Interactive Systems.mp3");
//    soundPlayer.play();

    physics.setSectorCount(SECTOR_COUNT);
    physics.setTimeStep(60);
    
    physics.setDrag(0.97f);
    physics.disableCollision();

    physics.addParticle(&fixedParticle);
    fixedParticle.setMass(1)->setRadius(10.f)->moveTo(ofPoint::zero())->makeFixed();

    ofxLoadCamera(previewCam, "preview_cam_settings");
    previewCam.setFov(camFov);
    previewCam.lookAt(fixedParticle.getPosition());
    
    bgImage.load("bg-light-gradient.png");
    soundStream.setup(this, 0, 2, 44100, 256, 4);
    
    // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
    fileName = "recording";
    fileExt = ".mov";
    // override the default codecs if you like
    // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
    vidRecorder.setVideoCodec("mpeg4");
    vidRecorder.setVideoBitrate("50000k");
    vidRecorder.setFfmpegLocation("/usr/local/bin/ffmpeg");
//    vidRecorder.setAudioCodec("mp3");
//    vidRecorder.setAudioBitrate("192k");
    bRecording = false;
    
    setupScreenFbo();
    
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    bRecording = false;
}

//-------------------------------------------------------------- 
void ofApp::setupGui(){
    
    settingsFileName = "settings.xml";
    
    float width = ofGetWindowWidth()/5;
    ofColor guiColor(0,0,0,255);
    
    gui.setup();
    gui.setSize(width, ofGetHeight());
    gui.setWidthElements(width);
    gui.setDefaultWidth(width);
    gui.setDefaultHeight(16);
    
    gui.add(fps.set("FPS", 0));
    
    ofParameterGroup    cameraParams;
    cameraParams.setName("CAMERA");
    cameraParams.add(orbitCamera.set("Orbit camera", false));
    cameraParams.add(camFov.set("Field of view", 60, 35.f, 180.f));
    cameraParams.add(camNearClip.set("Near clip", 0.1f, 0.1f, 20.f));
    cameraParams.add(camFarClip.set("Far clip", 5000.f, 20.f, 10000.f));
    gui.add(cameraParams);
    
    ofParameterGroup    light0Params;
    light0Params.setName("LIGHT 1");
    light0Params.add(enableLight0.set("Enabled", true));
    light0Params.add(orbitLight0.set("Orbit", true));
    light0Params.add(attConstant0.set("Constant Attenuation", 1.0, 0.0, 1.0));
    light0Params.add(attLinear0.set("Linear Attenuation", 0.0001, 0.0, 0.01));
    light0Params.add(attQuadratic0.set("Quadratic Attenuation", 0.0001, 0.0, 0.001));
    light0Params.add(lightAmbient0.set("Ambient", ofFloatColor(1,1,1,.1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    lightDiffuse0.set("Diffuse", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
    lightSpecular0.set("Specular", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
    gui.add(light0Params);
    
    ofParameterGroup    light1Params;
    light1Params.setName("LIGHT 2");
    light1Params.add(enableLight1.set("Enabled", true));
    light1Params.add(orbitLight1.set("Orbit", true));
    light1Params.add(attConstant1.set("Constant Attenuation", 1.0, 0.0, 1.0));
    light1Params.add(attLinear1.set("Linear Attenuation", 0.001, 0.0, 0.01));
    light1Params.add(attQuadratic1.set("Quadratic Attenuation", 0.0001, 0.0, 0.001));
    light1Params.add(lightAmbient1.set("Ambient", ofFloatColor(1,1,1,.1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    lightDiffuse1.set("Diffuse", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
    lightSpecular1.set("Specular", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
    gui.add(light1Params);
    
    gui.add(globalAmbient.set("Global ambient", ofFloatColor(.1,.1,.1), ofFloatColor(0,0,0), ofFloatColor(1,1,1)));
    
    ofParameterGroup    polygonParams;
    polygonParams.setName("POLYGONS");
    
    polygonParams.add(polygonAmbient.set("Ambient", ofFloatColor(1,1,1,.1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    polygonParams.add(polygonDiffuse.set("Diffuse", ofFloatColor(0.8,0.8,0.8,1.0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    polygonSpecular.set("Specular", ofFloatColor(0.8,0.8,0.8,1.0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
    polygonParams.add(polygonShininess.set("Shininess", 10, 0, 255));
    gui.add(polygonParams);
    
    ofParameterGroup    springParams;
    springParams.setName("SPRINGS");
    springParams.add(springAmbient.set("Ambient", ofFloatColor(1,1,1,.1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    springParams.add(springDiffuse.set("Diffuse", ofFloatColor(1.0,1.0,1.0,1.0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    springSpecular.set("Specular", ofFloatColor(0.8,0.8,0.8,1.0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
    springParams.add(springShininess.set("Shininess", 10, 0, 255));
    gui.add(springParams);
    
    ofParameterGroup    physicsParams;
    physicsParams.setName("PHYSICS");
    physicsParams.add(gravity.set("Gravity", ofPoint(0, 0, 0), ofPoint(-1, -1, -1), ofPoint(1, 1, 1)));
    physicsParams.add(attraction.set("Attraction", MIN_ATTRACTION, MIN_ATTRACTION, MAX_ATTRACTION));
    physicsParams.add(bindToFixedParticle.set("Bind to center", true));
    physicsParams.add(physicsPaused.set("Pause", false));
    physicsParams.add(radius.set("Radius", PARTICLE_MIN_RADIUS, PARTICLE_MIN_RADIUS, PARTICLE_MAX_RADIUS));
    physicsParams.add(mass.set("Mass", MIN_MASS, MIN_MASS, MAX_MASS));
    physicsParams.add(bounce.set("Bounce", MIN_BOUNCE, MIN_BOUNCE, MAX_BOUNCE));
    physicsParams.add(drag.set("Drag", 0.97, 0.0, 1.0));
    physicsParams.add(springStrength.set("Strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    physicsParams.add(springLength.set("Length", SPRING_MIN_LENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH));
    gui.add(physicsParams);
    
    gui.add(boxSize.set("Box size", 100.0, 1.0, 2000.0));
    gui.add(animateBoxSize.set("Animate box size", false));
    
    gui.add(lightOrbitSpeed.set("Light orbit speed", 0.01, 0.01, 1.0));
    gui.add(lightOrbitRadius.set("Light orbit radius", 1.0, 0.1, 2.0));
    
    gui.add(zDepth.set("Z depth", 50, 0, 400));
    gui.add(makeParticles.set("Make Particles", true));
    gui.add(makeSprings.set("Make Springs", true));
    gui.add(moveOnClick.set("Move on click", false));
    gui.add(fixedParticleMoveDuration.set("Click_Move duration", 0.3, 0.0, 10.0));
    gui.add(particleCount.set("Particle Count", 0));
    gui.add(springCount.set("Spring Count", 0));
    gui.add(attractionCount.set("Attraction Count", 0));
    gui.add(drawPolyMesh.set("Draw polygon mesh", true));
    gui.add(doNodeShader.set("Use node shader", true));
    gui.add(drawSpringMesh.set("Draw spring mesh", true));
    gui.add(drawWireframe.set("Draw wireframe", false));
    gui.add(drawLights.set("Draw lights", true));
    gui.add(drawGrid.set("Draw grid", true));
    gui.add(drawGui.set("Keep settings open", true));
    gui.add(audioEnabled.set("Audio enabled", false));
    
    fixedParticleMoveDuration.addListener(this, &ofApp::setFixedParticleMoveDuration);
    
    animateBoxSize.addListener(this, &ofApp::toggleAnimBoxSize);
    zDepth.addListener(this, &ofApp::setZDepth);
    boxSize.addListener(this, &ofApp::setPhysicsBoxSize);
    gravity.addListener(this, &ofApp::setGravityVec);
    camFov.addListener(this, &ofApp::setCamFov);
    camNearClip.addListener(this, &ofApp::setCamNearClip);
    camFarClip.addListener(this, &ofApp::setCamFarClip);
    audioEnabled.addListener(this, &ofApp::toggleAudio);
}

void ofApp::setupShading(){
    
    pLight0.setup();
    pLight1.setup();
    
    lights.push_back(pLight0);
    lights.push_back(pLight1);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetGlobalAmbientColor(globalAmbient);
    
    float time = ofGetElapsedTimef();
    float bs = boxSize / 2;
    float dt = 1.0f / 60.0f;
    int numParticles = physics.numberOfParticles();
    int numSprings = physics.numberOfSprings();
    int numAttractions = physics.numberOfAttractions();
    
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
    
    
    
    boxSizeAnimate.update(dt);
    if (boxSizeAnimate.isAnimating()) {
        boxSize.set(boxSizeAnimate.getCurrentValue());
    }
    
    fixedParticlePos.update(dt);
    fixedParticle.moveTo(fixedParticlePos.getCurrentPosition());
    
    fps.set(ofGetFrameRate());
    particleCount.set(numParticles);
    springCount.set(numSprings);
    attractionCount.set(numAttractions);

    if (orbitCamera) {
        float lng = time*10;
        float lat = sin(time/100);
        float radius = previewCam.getGlobalPosition().distance(previewCam.getTarget().getPosition());
        previewCam.orbit(lng, lat, radius);
    }
    
    if (enableLight0) {
        pLight0.enable();
        pLight0.setAttenuation(attConstant0, attLinear0, attQuadratic0);
        pLight0.setAmbientColor(lightAmbient0);
        pLight0.setDiffuseColor(lightDiffuse0);
        pLight0.setSpecularColor(lightSpecular0);
    } else {
        pLight0.disable();
    }
    if (enableLight1) {
        pLight1.enable();
        pLight1.setAttenuation(attConstant1, attLinear1, attQuadratic1);
        pLight1.setAmbientColor(lightAmbient1);
        pLight1.setDiffuseColor(lightDiffuse1);
        pLight1.setSpecularColor(lightSpecular1);
    } else {
        pLight1.disable();
    }
    if (orbitLight0 || orbitLight1) {
        float lat0 = sin(time*0.8*lightOrbitSpeed)*bs;
        float lng0 = cos(time*0.4*lightOrbitSpeed)*bs;
        float lat1 = cos(time*0.8*lightOrbitSpeed)*bs;
        float lng1 = sin(time*0.4*lightOrbitSpeed)*bs;
        float radius = boxSize * lightOrbitRadius;
        if (enableLight0 && orbitLight0)    pLight0.orbit(lng0, lat0, radius);
        if (enableLight1 && orbitLight1)    pLight1.orbit(lng1, lat1, radius);
    }
    
    if (enableLight0 || enableLight1) {
        polyMat.setAmbientColor(polygonAmbient);
        polyMat.setDiffuseColor(polygonDiffuse);
        polyMat.setSpecularColor(polygonSpecular);
        polyMat.setShininess(polygonShininess);
        
        springMat.setAmbientColor(springAmbient);
        springMat.setDiffuseColor(springDiffuse);
        springMat.setSpecularColor(springSpecular);
        springMat.setShininess(springShininess);
    }

    if (!physicsPaused) {
        physics.update();
    }
    
    if (drawPolyMesh) {
        
        float noiseScale = ofMap(mouseX, 0, ofGetWidth(), 0, 0.1);
        float noiseVel = ofGetElapsedTimef();

        
        polyMesh.clear();
        polyMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        
        for(int i=0; i<numParticles; i++){
            auto p = physics.getParticle(i);
            polyMesh.addVertex(p->getPosition());
            polyMesh.addColor(polyMat.getDiffuseColor());
        }
//        polyMesh.smoothNormals(1);
    }
    
    springMesh.clear();
    springMesh.setMode(OF_PRIMITIVE_LINES);
    for(int i=0; i<physics.numberOfSprings(); i++){
        
        auto spring = (msa::physics::Spring3D *) physics.getSpring(i);
        auto a = spring->getOneEnd();
        auto b = spring->getTheOtherEnd();
        ofVec3f vec = b->getPosition() - a->getPosition();
        float dist = vec.length();
        float angle = acos( vec.z / dist ) * RAD_TO_DEG;
        if(vec.z <= 0 ) angle = -angle;
        float rx = -vec.y * vec.z;
        float ry =  vec.x * vec.z;
        
        float size = ofMap(spring->getStrength(), SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH);
        
        springMesh.addVertex(a->getPosition());
        springMesh.addColor(springMat.getDiffuseColor());
        springMesh.addVertex(b->getPosition());
        springMesh.addColor(springMat.getDiffuseColor());
    }
    
    if (bRecording) {
        ofPixels pixels;
        screenFbo.readToPixels(pixels);
        bool success = vidRecorder.addFrame(pixels);
        if (!success) {
            ofLogWarning("This frame was not added!");
        }
    }
    
    // Check if the video recorder encountered any error while writing video frame or audio smaples.
    if (vidRecorder.hasVideoError()) {
        ofLogWarning("The video recorder failed to write some frames!");
    }
    
    if (vidRecorder.hasAudioError()) {
        ofLogWarning("The video recorder failed to write some audio samples!");
    }
    
    
    // Render to Fbo
    screenFbo.begin();
    ofClear(0,0,0,0);
    ofSetColor(ofColor::white);
    bgImage.draw(0, 0, FBO_WIDTH, FBO_HEIGHT);
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    previewCam.begin();
    if (enableLight0 || enableLight1) {
        ofEnableLighting();
        if (drawLights) {
            if (enableLight0) pLight0.draw();
            if (enableLight1) pLight1.draw();
        }
    }
    renderScene();
    previewCam.end();
    ofDisableDepthTest();
    ofDisableAlphaBlending();
    screenFbo.end();
    
    ofSetColor(ofColor::white);
    ofSetLineWidth(1 + (rms * 30.));
    waveform.draw();
}

//--------------------------------------------------------------
void ofApp::audioIn(float *input, int bufferSize, int nChannels){
    if (bRecording) {
        vidRecorder.addAudioSamples(input, bufferSize, nChannels);
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
void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recoded video file is now complete." << endl;
}

//--------------------------------------------------------------
void ofApp::setupScreenFbo(){
    if (!screenFbo.isAllocated()) {
        ofFbo::Settings settings;
        settings.numSamples = 4;
        settings.width = FBO_WIDTH;
        settings.height = FBO_HEIGHT;
        settings.internalformat = GL_RGB32F_ARB;
        settings.useDepth = true;
        settings.useStencil = true;
        screenFbo.allocate(settings);
    }
    screenFbo.begin();
    ofClear(0,0,0,0);
    screenFbo.end();
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
void ofApp::setPhysicsBoxSize(double& s){
    
//    springLength.setMax(s * 2);
    
    physics.setWorldSize(ofVec3f(-s, -s, -s),
                         ofVec3f(s, s, s));
    
    pLight0.setAreaLight(s/2, s/2);
    pLight1.setAreaLight(s/2, s/2);
    
//    pLight0.setPointLight();
//    pLight1.setPointLight();
}

//--------------------------------------------------------------
void ofApp::randomiseParams(){
    radius.set(ofRandom(radius.getMin(), radius.getMax()));
    mass.set(ofRandom(mass.getMin(), mass.getMax()));
    bounce.set(ofRandom(bounce.getMin(), bounce.getMax()));
    attraction.set(ofRandom(attraction.getMin(), attraction.getMax()));
    springStrength.set(ofRandom(springStrength.getMin(), springStrength.getMax()));
    springLength.set(ofRandom(springLength.getMin(), springLength.getMax()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(ofColor::white);
    
    screenFbo.draw((ofGetWidth()-FBO_WIDTH/2)/2, (ofGetHeight()-FBO_HEIGHT/2)/2, FBO_WIDTH/2, FBO_HEIGHT/2);
    ofDrawBitmapString(currentGraphName, drawGui ? gui.getWidth() + 20 : 20, 20);
    
    if (drawGui) {
        ofEnableAlphaBlending();
        gui.draw();
    }
    
    if (bRecording) {
        ofSetColor(255, 0, 0);
        ofDrawCircle(ofGetWidth() - 20, 20, 5);
    }
}

//--------------------------------------------------------------
void ofApp::renderScene(){
    if (drawGrid) {
        ofSetColor(255, 10);
        float stepSize = boxSize.get()/4;
        size_t numberOfSteps = 4;
        bool labels = false;
        ofDrawGrid(stepSize, numberOfSteps, labels);
    }
    
    if (drawPolyMesh) {
        // Draw polygon mesh
        polyMat.begin();
        if (drawWireframe)  polyMesh.drawWireframe();
        else                polyMesh.draw();
        polyMat.end();
        
    } else {
        polyMat.begin();
        for (int i=0; i<physics.numberOfParticles(); i++) {
            auto p = physics.getParticle(i);
            ofPushMatrix();
            ofTranslate(p->getPosition());
            float distToCam = p->getPosition().distance(previewCam.getGlobalPosition()) / 10;
            ofSetColor(255.0, ofClamp(255 - distToCam, 0, 255));
            ofDrawSphere(p->getRadius());
            ofPopMatrix();
        }
        polyMat.end();
    }
    if (drawSpringMesh) {
        //            springMat.begin();
        if (drawWireframe)  springMesh.drawWireframe();
        else                springMesh.draw();
        //            springMat.end();
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();
    
    animateBoxSize.removeListener(this, &ofApp::toggleAnimBoxSize);
    zDepth.removeListener(this, &ofApp::setZDepth);
    fixedParticleMoveDuration.removeListener(this, &ofApp::setFixedParticleMoveDuration);
    boxSize.removeListener(this, &ofApp::setPhysicsBoxSize);
    gravity.removeListener(this, &ofApp::setGravityVec);
    camFov.removeListener(this, &ofApp::setCamFov);
    camNearClip.removeListener(this, &ofApp::setCamNearClip);
    camFarClip.removeListener(this, &ofApp::setCamFarClip);
    audioEnabled.removeListener(this, &ofApp::toggleAudio);
    ofxSaveCamera(previewCam, "preview_cam_settings");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {

        case 'c':
            makeCluster();
            break;
        case 'o':
            orbitCamera = !orbitCamera;
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
        
        case ' ': {
            physics.clear();
            physics.addParticle(&fixedParticle);
            setupScreenFbo();
            break;
        }
        case '1':
            makeParticles = !makeParticles;
            break;
        case '2':
            makeSprings = !makeSprings;
            break;
        
        case '.':
            randomiseParams();
            break;
        case 'Q': {
            polyMesh.save("polyMesh.ply");
            springMesh.save("springMesh.ply");
        }
            break;
        
        case 'r': {
            bRecording = !bRecording;
            if(bRecording && !vidRecorder.isInitialized()) {
                vidRecorder.setup(fileName+ofGetTimestampString()+fileExt,
                                  FBO_WIDTH, FBO_HEIGHT,
                                  60, 44100, 2, false, false);
                vidRecorder.start();
            }
            else if(!bRecording && vidRecorder.isInitialized()) {
                vidRecorder.setPaused(true);
            }
            else if(bRecording && vidRecorder.isInitialized()) {
                vidRecorder.setPaused(false);
            }
            
            break;
        }
        case 'R': {
            bRecording = false;
            vidRecorder.close();
            break;
        }
            
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
    if (moveOnClick && button == 0) {
        int n = physics.numberOfParticles();
        float nearestDistance = 0;
        ofVec2f nearestVertex;
        int nearestIndex = 0;
        ofVec2f mouse(x, y);
        for(int i = 0; i < n; i++) {
            ofVec3f cur = previewCam.worldToScreen(physics.getParticle(i)->getPosition());
            float distance = cur.distance(mouse);
            if(i == 0 || distance < nearestDistance) {
                nearestDistance = distance;
                nearestVertex = cur;
                nearestIndex = i;
            }
        }
        ofPoint pos = physics.getParticle(nearestIndex)->getPosition();
        previewCam.lookAt(pos);
//        fixedParticlePos.animateTo(physics.getParticle(nearestIndex)->getPosition());
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

//--------------------------------------------------------------
template <typename T>
void ofApp::makeSpringBetweenParticles(ParticleT<T> *a, ParticleT<T> *b) {
    float dist = a->getPosition().distance(b->getPosition());
    float strength = dist*springStrength;
    bool springExists = false;
    for (int i=0; i<physics.numberOfSprings(); i++) {
        auto s = physics.getSpring(i);
        if ((s->getOneEnd() == a || s->getTheOtherEnd() == a) &&
            (s->getOneEnd() == b || s->getTheOtherEnd() == b)) {
            springExists = true;
            return;
        }
    }
    if (!springExists) physics.makeSpring(a, b, springStrength, springLength);
}

//--------------------------------------------------------------
void ofApp::makeParticleAtCenter(float r){
    auto a = new Particle3D;
    a->setMass(mass)
    ->setBounce(bounce)
    ->setRadius(radius)
    ->enableCollision()
    ->makeFree()
    ->moveTo(ofPoint(ofRandom(-r, r),
                     ofRandom(-r, r),
                     ofRandom(-r, r)));

    physics.addParticle(a);
}

//--------------------------------------------------------------
void ofApp::makeParticleAtPosition(const ofPoint& p){
    auto a = new Particle3D;
    a->setMass(mass)
    ->setBounce(bounce)
    ->setRadius(radius)
    ->enableCollision()
    ->makeFree()
    ->moveTo(p);
    physics.addParticle(a);
    
    if (attraction > 0.0f) {
        physics.makeAttraction(a, &fixedParticle, attraction);
    }
    

    for (int i = 0; i < physics.numberOfParticles(); i++) {
        float dist = physics.getParticle(i)->getPosition().distance(a->getPosition());
//        if (dist > SPRING_MIN_LENGTH) {
//        physics.makeAttraction(a, physics.getParticle(i), attraction);
//        }
    }

    a->release();
}

//--------------------------------------------------------------
void ofApp::makeCluster(){
    
    int numParticles = physics.numberOfParticles();
    
    if (makeParticles) {
        //            auto pos = previewCam.screenToWorld(ofVec3f(x, y, 0));
        //            makeParticleAtPosition(pos);
        makeParticleAtCenter(boxSize * 0.8f);
    }
    
    if (makeSprings && numParticles > 1) {
        for (int i = numParticles; i > 0; i--) {
            auto a = physics.getParticle(i-1);
            auto b = physics.getParticle(i);
            
            if (numParticles % 2 == 0) {
                if (bindToFixedParticle) {
                    makeSpringBetweenParticles(a, &fixedParticle);
                    makeSpringBetweenParticles(b, &fixedParticle);
                }
                makeSpringBetweenParticles(a, b);
            }
        }
    }
    if (numParticles > 1 && attraction > 0.0f) {
        auto a = physics.getParticle(numParticles-1);
        for (int i=0; i<numParticles-1; i++) {
            auto b = physics.getParticle(i);
            physics.makeAttraction(a, b, attraction);
        }
    }
}
