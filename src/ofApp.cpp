#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofSetSmoothLighting(true);
    ofSetGlobalAmbientColor(ofColor(0, 0, 0));
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    float width = ofGetWidth();
    float height = ofGetHeight();

    resetCamera();
    setupGui();
    gui.minimizeAll();
    setupShading();

    physics.setSectorCount(SECTOR_COUNT);
//    physics.setTimeStep(60);
    physics.setDrag(0.97f);
    physics.setDrag(1);
    physics.enableCollision();

    physics.addParticle(&fixedParticle);
    fixedParticle.setMass(1)->setRadius(10.f)->moveTo(ofPoint::zero())->makeFixed();

    ofxLoadCamera(previewCam, "preview_cam_settings");
    previewCam.setFov(camFov);
    previewCam.lookAt(fixedParticle.getPosition());

    polyTextureImage.load("1380489_10152539373427814_159706546_n.jpg");
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
    
    gui.setDefaultBackgroundColor(guiColor);
//    gui.setDefaultFillColor(fillColor);
    gui.setDefaultHeaderBackgroundColor(guiColor);
    gui.setDefaultBorderColor(guiColor);
//    gui.setDefaultTextColor(ofColor::black);
    
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
    light0Params.add(attLinear0.set("Linear Attenuation", 0.001, 0.0001, 0.1));
    light0Params.add(attQuadratic0.set("Quadratic Attenuation", 0.0001, 0.0001, 0.001));
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
    physicsParams.add(spring_strength.set("Strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    physicsParams.add(spring_length.set("Length", SPRING_MIN_LENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH));
    gui.add(physicsParams);
    
    gui.add(boxSize.set("Box size", 100.f, 1.f, 2000.f));
    gui.add(makeParticles.set("Make Particles", true));
    gui.add(makeSprings.set("Make Springs", true));
    gui.add(particleCount.set("Particle Count", "0"));
    gui.add(springCount.set("Spring Count", "0"));
    gui.add(attractionCount.set("Attraction Count", "0"));
    gui.add(drawUsingVboMesh.set("Draw mesh", true));
    gui.add(drawSprings.set("Draw springs", true));
    gui.add(drawWireframe.set("Draw wireframe", false));
    gui.add(drawLights.set("Draw lights", true));
    gui.add(drawGrid.set("Draw grid", true));
    gui.add(useLeap.set("LeapMotion", false));
    gui.add(drawGui.set("Keep settings open", true));

    useLeap.addListener(this, &ofApp::toggleLeap);
    boxSize.addListener(this, &ofApp::setPhysicsBoxSize);
    gravity.addListener(this, &ofApp::setGravityVec);
    camFov.addListener(this, &ofApp::setCamFov);
    camNearClip.addListener(this, &ofApp::setCamNearClip);
    camFarClip.addListener(this, &ofApp::setCamFarClip);
}

void ofApp::setupShading(){
    
    pLight0.setup();
    pLight0.setAreaLight(boxSize/4, boxSize/4);
    pLight0.setAttenuation(0.1,0.01,0.001);
    
    pLight1.setup();
    pLight1.setAreaLight(boxSize/4, boxSize/4);
//    pLight1.setSpotlight(0.8, 0.001);
    pLight1.setAttenuation(.25,0.001,0.0001);
}

//--------------------------------------------------------------
void ofApp::update(){

    float time = ofGetElapsedTimef();
    float bs = boxSize / 2;
    
    particleCount.set(ofToString(physics.numberOfParticles()));
    springCount.set(ofToString(physics.numberOfSprings()));
    attractionCount.set(ofToString(physics.numberOfAttractions()));

    if (orbitCamera) {
        float lng = time*10;
        float lat = sin(time*boxSize/1000)*10;
//        float radius = sin(time*boxSize/2000)*50 + 600;
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
        float lat0 = sin(time*0.4)*bs;
        float lng0 = cos(time*0.2)*bs;
        float lat1 = cos(time*0.4)*bs;
        float lng1 = sin(time*0.2)*bs;
        float radius = boxSize * 0.8;
        if (enableLight0 && orbitLight0)    pLight0.orbit(lng0, lat0, radius);
        if (enableLight1 && orbitLight1)    pLight1.orbit(lng1, lat1, radius);
    }
    
    polyMat.setAmbientColor(polygonAmbient);
    polyMat.setDiffuseColor(polygonDiffuse);
    polyMat.setSpecularColor(polygonSpecular);
    polyMat.setShininess(polygonShininess);
    
    springMat.setEmissiveColor(springAmbient);
    springMat.setAmbientColor(springAmbient);
    springMat.setDiffuseColor(springDiffuse);
    springMat.setSpecularColor(springSpecular);
    springMat.setShininess(springShininess);


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
                        ->makeFree()
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

        polyMesh.clear();
        polyMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        for(int i=0; i<physics.numberOfParticles(); i++){
            auto p = physics.getParticle(i);
            polyMesh.addVertex(p->getPosition());
            polyMesh.addColor(polyMat.getDiffuseColor());
            polyMesh.addVertex(p->getPosition());
            polyMesh.addColor(polyMat.getDiffuseColor());
            polyMesh.addVertex(p->getPosition());
            polyMesh.addColor(polyMat.getDiffuseColor());
            polyMesh.addTexCoord(ofVec2f(p->getPosition()/10));
            polyMesh.addTexCoord(ofVec2f(p->getPosition()/10));
            polyMesh.addTexCoord(ofVec2f(p->getPosition()/10));
            
            polyMesh.addIndex(i-1);
            polyMesh.addIndex(i);
            polyMesh.addIndex(i+1);
        }

        springMesh.clear();
        springMesh.setMode(OF_PRIMITIVE_LINE_LOOP);
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

//            ofRotate(angle, rx, ry, 0.0);
            float size = ofMap(spring->getStrength(), SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH);

            springMesh.addVertex(a->getPosition());
            springMesh.addColor(springMat.getDiffuseColor());
            springMesh.addVertex(b->getPosition());
            springMesh.addColor(springMat.getDiffuseColor());
            
            if (i == 0) {
                polyMesh.addIndex(i);
                polyMesh.addIndex(physics.numberOfSprings() - 1);
            } else if (i == physics.numberOfSprings() - 1) {
                springMesh.addIndex(i);
                springMesh.addIndex(0);
            } else {
                springMesh.addIndex(i);
                springMesh.addIndex(i+1);
            }
        }
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
    spring_length.setMax(s*2);
    physics.setWorldSize(ofVec3f(-s, -s, -s),
                         ofVec3f(s, s, s));
//    physics.clearWorldSize();
}
void ofApp::setGravityVec(ofPoint& g){
    physics.setGravity(g);
}
void ofApp::setCamFov(float& v) {
    previewCam.setFov(v);
}
void ofApp::setCamNearClip(float& v) {
    previewCam.setNearClip(v);
}
void ofApp::setCamFarClip(float& v) {
    previewCam.setFarClip(v);
}

void ofApp::resetCamera(){
//    previewCam.reset();
//    previewCam.setDistance(5.0f);
//    previewCam.setNearClip(0.01f);
//    previewCam.setFarClip(5000.0f);
//    previewCam.setPosition(0.4f, 0.2f, 0.8f);
//    previewCam.lookAt(ofVec3f(0.0f, 0.0f, 0.0f));
}

void ofApp::randomiseParams(){
    radius.set(ofRandom(radius.getMin(), radius.getMax()));
    mass.set(ofRandom(mass.getMin(), mass.getMax()));
    bounce.set(ofRandom(bounce.getMin(), bounce.getMax()));
    attraction.set(ofRandom(attraction.getMin(), attraction.getMax()));
    spring_strength.set(ofRandom(spring_strength.getMin(), spring_strength.getMax()));
    spring_length.set(ofRandom(spring_length.getMin(), spring_length.getMax()));
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofEnableDepthTest();
    ofEnableAlphaBlending();
    float width = ofGetWidth();
    float height = ofGetHeight();

    ofPushMatrix();
    previewCam.begin();
    if (enableLight0 || enableLight1) {
        ofEnableLighting();
    }

    if (drawGrid) {
        ofSetColor(255, 50);
        float stepSize = boxSize.get()/4;
        size_t numberOfSteps = 4;
        bool labels = false;
        ofDrawGrid(stepSize, numberOfSteps, labels);
//        ofDrawAxis(boxSize.get());
    }

    if (drawUsingVboMesh) {
        // Draw polygon mesh
        polyMat.begin();
//        polyTextureImage.bind();
        if (drawWireframe)  polyMesh.drawWireframe();
        else                polyMesh.draw();
//        polyTextureImage.unbind();
        polyMat.end();
        
        if (drawSprings) {
            springMat.begin();
            glLineWidth(4);
            if (drawWireframe)  springMesh.drawWireframe();
            else                springMesh.draw();
            springMat.end();
        }

    } else {
        // Draw polygons
        for(int i=0; i<physics.numberOfParticles(); i++){
            auto p = physics.getParticle(i);
            ofPushMatrix();
            ofTranslate(p->getPosition());
            ofSetColor(200);
            ofSetCircleResolution(p->getRadius()*10);
            polyMat.begin();
            if (p->isFree())
                ofDrawSphere(p->getRadius());
            polyMat.end();
            ofPopMatrix();
        }
        // Draw springs
        ofFloatColor(0.5, 0.5, 0.5, 0.5);
        for(int i=0; i<physics.numberOfSprings(); i++) {
            auto spring = (msa::physics::Spring3D *) physics.getSpring(i);
            auto a = spring->getOneEnd();
            auto b = spring->getTheOtherEnd();

            springMat.begin();
            ofDrawLine(a->getPosition(), b->getPosition());
            springMat.end();
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

    if (enableLight0 || enableLight1) {
        if (drawLights) {
            if (enableLight0) pLight0.draw();
            if (enableLight1) pLight1.draw();
        }
        ofDisableLighting();
    }
    previewCam.end();
    ofPopMatrix();
    ofDisableDepthTest();
    
    if (drawGui) {
        gui.draw();
    }
}

void ofApp::exit(){
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
    float strength = dist*spring_strength;
    physics.makeSpring(a, b, spring_strength, spring_length);
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

    physics.makeAttraction(a, &fixedParticle, attraction);

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
        for (int i=numParticles; i>0; i--) {
            auto a = physics.getParticle(i-1);
            auto b = physics.getParticle(i);
            
            if (makeSprings && numParticles % 2 == 0) {
                if (bindToFixedParticle) {
                    makeSpringBetweenParticles(a, &fixedParticle);
                    makeSpringBetweenParticles(b, &fixedParticle);
                }
                makeSpringBetweenParticles(a, b);
            }
        }
    }
    if (numParticles > 1) {
        auto a = physics.getParticle(numParticles-1);
        for (int i=0; i<numParticles-1; i++) {
            auto b = physics.getParticle(i);
            physics.makeAttraction(a, b, attraction);
        }
    }
}
