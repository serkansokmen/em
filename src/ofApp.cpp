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
    ofSetCircleResolution(20);
    ofSetGlobalAmbientColor(ofFloatColor(0.1, 0.1, 0.1));

    float width = ofGetWidth();
    float height = ofGetHeight();

    setupShading();
    setupGui();
    gui.minimizeAll();
    restoreParams();
    
    fixedParticlePos.setPosition(ofPoint(0,0,0));
    fixedParticlePos.setRepeatType(PLAY_ONCE);
    fixedParticlePos.setCurve(EXPONENTIAL_SIGMOID_PARAM);
    
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
    
//    gui.setDefaultBackgroundColor(guiColor);
//    gui.setDefaultFillColor(fillColor);
//    gui.setDefaultHeaderBackgroundColor(guiColor);
//    gui.setDefaultBorderColor(guiColor);
//    gui.setDefaultTextColor(ofColor::black);
    
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
    light0Params.add(attLinear0.set("Linear Attenuation", 0.0001, 0.0, 0.1));
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
    light1Params.add(attLinear1.set("Linear Attenuation", 0.001, 0.0001, 0.1));
    light1Params.add(attQuadratic1.set("Quadratic Attenuation", 0.0001, 0.0001, 0.001));
    light1Params.add(lightAmbient1.set("Ambient", ofFloatColor(1,1,1,.1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    lightDiffuse1.set("Diffuse", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
    lightSpecular1.set("Specular", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
    gui.add(light1Params);
    
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
    physicsParams.add(springStrength.set("Strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    physicsParams.add(springLength.set("Length", SPRING_MIN_LENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH));
    gui.add(physicsParams);
    
    gui.add(boxSize.set("Box size", 100.f, 1.f, 2000.f));
    gui.add(lightOrbitSpeed.set("Light orbit speed", 0.01, 0.01, 1.0f));
    gui.add(zDepth.set("Z depth", 50, 0, 400));
    gui.add(makeParticles.set("Make Particles", true));
    gui.add(makeSprings.set("Make Springs", true));
    gui.add(moveOnClick.set("Move on click", false));
    gui.add(fixedParticleMoveDuration.set("Click_Move duration", 0.3, 0.0, 10.0));
    gui.add(particleCount.set("Particle Count", 0));
    gui.add(springCount.set("Spring Count", 0));
    gui.add(attractionCount.set("Attraction Count", 0));
    gui.add(drawUsingVboMesh.set("Draw mesh", true));
    gui.add(drawSprings.set("Draw springs", true));
    gui.add(drawWireframe.set("Draw wireframe", false));
    gui.add(drawLights.set("Draw lights", true));
    gui.add(drawGrid.set("Draw grid", true));
    gui.add(drawLabels.set("Draw names", true));
    gui.add(useLeap.set("LeapMotion", false));
    gui.add(drawGui.set("Keep settings open", true));
    
    fixedParticleMoveDuration.addListener(this, &ofApp::setFixedParticleMoveDuration);
    
    zDepth.addListener(this, &ofApp::setZDepth);
    useLeap.addListener(this, &ofApp::toggleLeap);
    boxSize.addListener(this, &ofApp::setPhysicsBoxSize);
    gravity.addListener(this, &ofApp::setGravityVec);
    camFov.addListener(this, &ofApp::setCamFov);
    camNearClip.addListener(this, &ofApp::setCamNearClip);
    camFarClip.addListener(this, &ofApp::setCamFarClip);
}

void ofApp::setupShading(){
    
    pLight0.setup();
    pLight1.setup();
    
    lights.push_back(pLight0);
    lights.push_back(pLight1);
    
    shader.setGeometryInputType(GL_LINES);
    shader.setGeometryOutputType(GL_TRIANGLE_STRIP);
    shader.setGeometryOutputCount(4);
    shader.load("shaders/sphere.vert", "shaders/sphere.frag");
    
//    polyMesh.setUsage(GL_STATIC_DRAW);
//    springMesh.setUsage(GL_STATIC_DRAW);
//    nodeMesh.setUsage(GL_STATIC_DRAW);
}

//--------------------------------------------------------------
void ofApp::update(){

    float time = ofGetElapsedTimef();
    float bs = boxSize / 2;
    float dt = 1.0f / 60.0f;
    int numParticles = physics.numberOfParticles();
    int numSprings = physics.numberOfSprings();
    int numAttractions = physics.numberOfAttractions();
    
    fixedParticlePos.update(dt);
//    if (!fixedParticlePos.isAnimating()) {
    fixedParticle.moveTo(fixedParticlePos.getCurrentPosition());
//    }

    
    fps.set(ofGetFrameRate());
    particleCount.set(numParticles);
    springCount.set(numSprings);
    attractionCount.set(numAttractions);

    if (orbitCamera) {
        float lng = time*10;
        float lat = sin(time*boxSize/1000)*2;
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
        float radius = boxSize * 1.2;
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

    if (useLeap) {
        Leap::GestureList gestures = leap.frame().gestures();

        float tweenVal = (ofGetElapsedTimeMillis() % 2000) / 2000.f;
        bool step = floor(((ofGetElapsedTimeMillis() % 500) / 500.f) * 100.f) < 10;

        for (auto gesture : gestures) {
            switch (gesture.type()) {

                case Leap::Gesture::TYPE_CIRCLE: {
                    ofLog() << "Circle gesture detected" << endl;
                    for (auto pointable : gesture.pointables()) {
                        ofPoint tipPos(pointable.stabilizedTipPosition().x,
                                       pointable.stabilizedTipPosition().y,
                                       pointable.stabilizedTipPosition().z);
                        ofPoint tipVel(pointable.tipVelocity().x,
                                       pointable.tipVelocity().y,
                                       pointable.tipVelocity().z);

                        auto p = new Particle3D;
                        p->setMass(mass)
                        ->setBounce(bounce)
                        ->setRadius(radius)
                        ->enableCollision()
                        ->makeFixed()
                        ->moveTo(tipPos)
                        ->addVelocity(tipVel);
                        physics.addParticle(p);
                        if (bindToFixedParticle) {
                            makeSpringBetweenParticles(p, &fixedParticle);
                        }
                        p->release();
                    }
                    break;
                }

                case Leap::Gesture::TYPE_SCREEN_TAP: {
                    break;
                }

                case Leap::Gesture::TYPE_SWIPE: {
                    Leap::SwipeGesture swipe = (Leap::SwipeGesture)gesture;
                    Leap::Vector swipeDir = swipe.direction();
                    ofPoint swipePoint(swipeDir.x, swipeDir.y, swipeDir.z);
                    swipePoint.normalize();

                    ofQuaternion startQuat, targetQuat;
                    ofPoint startPos, targetPos;

                    startQuat.set(previewCam.getOrientationQuat());
                    startPos.set(previewCam.getGlobalPosition());
                    targetQuat.set(startQuat);
                    targetPos.set(startPos);

                    if (swipePoint.x < 0) {
//                        targetQuat.makeRotate(10, 0, 1, 0);
                        targetPos.x -= 50.f;
                    } else if (swipePoint.x > 0) {
//                        targetQuat.makeRotate(-10, 0, 1, 0);
                        targetPos.x += 50.f;
                    }

                    ofQuaternion    tweenedCamQuat;
                    ofPoint         lerpPos;

                    tweenedCamQuat.slerp(tweenVal, startQuat, targetQuat);
                    lerpPos = startPos + ((targetPos - startPos) * tweenVal);

//                    previewCam.setOrientation(tweenedCamQuat);
//                    previewCam.setGlobalPosition(lerpPos);
//                    previewCam.lookAt(fixedParticle.getPosition());
                    previewCam.truck(swipePoint.x);

                    break;
                }
                case Leap::Gesture::TYPE_KEY_TAP:
//                    makeCluster();
                    break;

                default:
                    break;
            }
        }
    }

    if (!physicsPaused) {
        physics.update();
    }
    
    if (drawUsingVboMesh) {
        
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
    } else {
        
        nodeMesh.clear();
        for(int i=0; i<numParticles; i++){
            auto p = physics.getParticle(i);
            nodeMesh.addVertex(p->getPosition());
//            nodeMesh.addTexCoord(p->getPosition());
            nodeMesh.addColor(ofFloatColor(1,1,1,1));
        }
        
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
}

//--------------------------------------------------------------
void ofApp::restoreParams(){
    gui.loadFromFile(settingsFileName);
}

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
void ofApp::toggleLeap(bool &v){
    if (leap.isConnected()) {
        leap.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
        leap.enableGesture(Leap::Gesture::TYPE_SWIPE);
        leap.enableGesture(Leap::Gesture::TYPE_CIRCLE);
        leap.config().setFloat("Gesture.Swipe.MinLength", 100.0f);
        leap.config().setFloat("Gesture.Swipe.MinVelocity", 250);
        leap.config().setFloat("Gesture.ScreenTap.MinForwardVelocity", 30.0);
        leap.config().setFloat("Gesture.ScreenTap.HistorySeconds", .5);
        leap.config().setFloat("Gesture.ScreenTap.MinDistance", 1.0);
        leap.config().save();
    }
}

void ofApp::setPhysicsBoxSize(double& s){
    springLength.setMax(s*2);
//    physics.setWorldSize(ofVec3f(-s, -s, -s),
//                         ofVec3f(s, s, s));
//    pLight0.setAreaLight(s/2, s/2);
//    pLight1.setAreaLight(s/2, s/2);
    pLight0.setPointLight();
    pLight1.setPointLight();
//    physics.clearWorldSize();
}

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

    float width = ofGetWidth();
    float height = ofGetHeight();
    
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
    
    ofSetColor(ofColor::white);
    ofDrawBitmapString(currentGraphName, drawGui ? gui.getWidth() + 20 : 20, 20);
    
    if (drawGui) {
        ofEnableAlphaBlending();
        gui.draw();
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
    
    if (drawUsingVboMesh) {
        // Draw polygon mesh
        polyMat.begin();
        if (drawWireframe)  polyMesh.drawWireframe();
        else                polyMesh.draw();
        polyMat.end();
        
        if (drawSprings) {
//            springMat.begin();
            if (drawWireframe)  springMesh.drawWireframe();
            else                springMesh.draw();
//            springMat.end();
        }
        
    } else {
        // Draw spheres mesh
        shader.begin();
//        shader.setUniform3f("position", 0, 0, 0);
        nodeMesh.draw();
        shader.end();
        if (drawSprings) {
            springMesh.draw();
        }
    }
    
    if (drawLabels) {
        for (int i=0; i<physics.numberOfParticles(); i++) {
            auto p = physics.getParticle(i);
            ofPushMatrix();
            ofTranslate(p->getPosition());
            float distToCam = p->getPosition().distance(previewCam.getGlobalPosition()) / 10;
            ofSetColor(255.0, ofClamp(255 - distToCam, 0, 255));
//            ofDrawBitmapString(p->node.name, 0, 0);
            ofPopMatrix();
        }
    }
    
    if (useLeap) {
        Leap::PointableList pointables = leap.frame().pointables();
        //        Leap::GestureList gestures = leap.frame().gestures();
        Leap::InteractionBox iBox = leap.frame().interactionBox();
        
        for (int p = 0; p < pointables.count(); p++){
            
            Leap::Pointable pointable = pointables[p];
            Leap::Vector normalizedPosition = iBox.normalizePoint(pointable.stabilizedTipPosition());
            
            ofPoint tipPos(pointable.stabilizedTipPosition().x,
                           pointable.stabilizedTipPosition().y,
                           pointable.stabilizedTipPosition().z);
            ofPoint normBoxPos(normalizedPosition.x,
                               normalizedPosition.y,
                               normalizedPosition.z);
            
            if (pointable.touchDistance() > 0 &&
                pointable.touchZone() != Leap::Pointable::Zone::ZONE_NONE){
                ofSetColor(0, 255, 0, (1 - pointable.touchDistance())*255.f + 150.f);
            }
            else if (pointable.touchDistance() <= 0){
                ofSetColor(255, 0, 0, -pointable.touchDistance()*255 + 100.f);
            }
            else {
                ofSetColor(0, 0, 255, 255);
            }
            
            //            ofDrawSphere(normBoxPos*boxSize, 10);
            
            ofPushMatrix();
            ofTranslate(tipPos);
            ofDrawSphere(0, 0, 10);
            ofPopMatrix();
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    zDepth.removeListener(this, &ofApp::setZDepth);
    fixedParticleMoveDuration.removeListener(this, &ofApp::setFixedParticleMoveDuration);
    useLeap.removeListener(this, &ofApp::toggleLeap);
    boxSize.removeListener(this, &ofApp::setPhysicsBoxSize);
    gravity.removeListener(this, &ofApp::setGravityVec);
    camFov.removeListener(this, &ofApp::setCamFov);
    camNearClip.removeListener(this, &ofApp::setCamNearClip);
    camFarClip.removeListener(this, &ofApp::setCamFarClip);
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
        case ',':
            drawGui = !drawGui;
        case 'm':
            gui.minimizeAll();
            break;
        case 'n':
            gui.maximizeAll();
            break;
        case '=':
            loadJson("7f0b54a4-02b3-4e83-bd68-4c007f9c2dfd.json");
            break;
        case '-':
            loadJson("1ac1ee14-0567-4f04-a6e5-96b24ed5b5d5.json");
            break;
        
        case ' ': {
            physics.clear();
            physics.addParticle(&fixedParticle);
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

//--------------------------------------------------------------
void ofApp::loadJson(const string& url){
    
    // Now parse the JSON
    bool parsingSuccessful = json.open(url);
    
    if (parsingSuccessful){
        
        nodeTypes.clear();
        edgeTypes.clear();
        nodes.clear();
        edges.clear();
        physics.clear();
        
        currentGraphName = json["graph"]["name"].asString();
        
        // Parse edge types
        for (Json::ArrayIndex i = 0; i < json["graph"]["edge_types"].size(); ++i){
            auto jsNode = json["graph"]["edge_types"][i];
            gcv::EdgeType it;
            it.id = jsNode["id"].asString();
            it.name = jsNode["name"].asString();
            it.description = jsNode["description"].asString();
            it.weighted = jsNode["weighted"].asDouble();
            it.directed = jsNode["directed"].asDouble();
            it.color = getHexFromColorName(jsNode["color"].asString());
            edgeTypes.push_back(it);
        }
        
        // Parse node types
        for (Json::ArrayIndex i = 0; i < json["graph"]["node_types"].size(); ++i){
            auto jsNode = json["graph"]["node_types"][i];
            gcv::NodeType it;
            it.id = jsNode["id"].asString();
            it.name = jsNode["name"].asString();
            it.description = jsNode["description"].asString();
            it.image = jsNode["image"].asString();
            it.color = getHexFromColorName(jsNode["color"].asString());
            nodeTypes.push_back(it);
        }
        
        // Parse nodes
        for (Json::ArrayIndex i=0; i<json["graph"]["nodes"].size(); ++i){
            auto jsNode = json["graph"]["nodes"][i];
            gcv::Node node;
            node.id = jsNode["id"].asString();
            node.name = jsNode["name"].asString();
            node.pos_x = jsNode["pos_x"].asDouble();
            node.pos_y = jsNode["pos_y"].asDouble();
            node.type_id = jsNode["type_id"].asString();
            nodes.push_back(node);
            
            makeParticleForNode(node);
        }
        
        // Parse edges
        for (Json::ArrayIndex i=0; i<json["graph"]["edges"].size(); ++i){
            auto jsNode = json["graph"]["edges"][i];
            gcv::Edge edge;
            edge.id = jsNode["id"].asString();
            edge.name = jsNode["name"].asString();
            edge.from = jsNode["from"].asString();
            edge.to = jsNode["to"].asString();
            edge.type_id = jsNode["type_id"].asString();            
            edges.push_back(edge);
            
//            makeSpringForEdge(edge);
        }
        
    } else {
        ofLogNotice("ofApp::loadJson") << "Failed to parse JSON.";
    }
}

void ofApp::makeParticleForNode(const gcv::Node& node){
    auto a = new Particle3D;
    a->setMass(mass)
    ->setBounce(bounce)
    ->setRadius(radius)
    ->enableCollision()
    ->makeFree()
    ->moveTo(ofPoint(node.pos_x, node.pos_y, ofRandom(-zDepth, zDepth)));
    //            a->node = node;
    for (auto nt : nodeTypes) {
        if (nt.id == node.type_id) {
            //                    a->color = ofColor(ofHexToInt(nt.color));
        }
    }
    physics.addParticle(a);
}

void ofApp::makeSpringForEdge(const gcv::Edge& edge) {
    Particle3D *a;
    Particle3D *b;
    for (int i=0; i<physics.numberOfParticles(); ++i) {
        auto p = physics.getParticle(i);
//        if (p->node.id == edge.from) {
//            a = p;
//        }
//        if (p->node.id == edge.to) {
//            b = p;
//        }
    }

    if (a != NULL && b != NULL) {
        bool springExists = false;
        float dist = a->getPosition().distance(b->getPosition());
        for (int i=0; i<physics.numberOfSprings(); i++) {
            auto s = physics.getSpring(i);
            if ((s->getOneEnd() == a || s->getTheOtherEnd() == a) &&
                (s->getOneEnd() == b || s->getTheOtherEnd() == b)) {
                springExists = true;
                return;
            }
        }
        if (!springExists) physics.makeSpring(a, b, 1.0, dist);
    }
}