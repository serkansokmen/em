#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofSetSmoothLighting(true);

    float width = ofGetWidth();
    float height = ofGetHeight();

    ofFloatColor c;
    c.setHsb(1, 1, 1);
    lightColor0.set(c);
    lightColor1.set(c);
    
    polyMat.setShininess(255);
    polyMat.setEmissiveColor(ofFloatColor(.0,.0,.0));
    polyMat.setSpecularColor(c);
    
    springMat.setSpecularColor(c);
    springMat.setShininess(255);
    
    resetCamera();
    setupGui();
    
    physics.setSectorCount(SECTOR_COUNT);
    // physics.setTimeStep(60);
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

    gui.setup();
    drawGui.set(true);
    
    float w = ofGetWindowWidth()/4;
    gui.setSize(w, ofGetHeight());
    gui.setWidthElements(w);
    gui.setDefaultWidth(w);
    gui.setDefaultHeight(20);
    
    ofParameterGroup    physicsParams;
    ofParameterGroup    renderParams;
    ofParameterGroup    debugParams;
    ofParameterGroup    cameraParams;
    
    physicsParams.setName("PHYSICS");
    physicsParams.add(boxSize.set("world size", 100.f, 1.f, 2000.f));
    physicsParams.add(makeParticles.set("particles", true));
    physicsParams.add(makeSprings.set("springs", true));
    physicsParams.add(radius.set("particle radius", PARTICLE_MIN_RADIUS, PARTICLE_MIN_RADIUS, PARTICLE_MAX_RADIUS));
    physicsParams.add(mass.set("particle mass", MIN_MASS, MIN_MASS, MAX_MASS));
    physicsParams.add(bounce.set("particle bounce", MIN_BOUNCE, MIN_BOUNCE, MAX_BOUNCE));
    physicsParams.add(attraction.set("particle attraction", MIN_ATTRACTION, MIN_ATTRACTION, MAX_ATTRACTION));
    physicsParams.add(spring_strength.set("spring strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    physicsParams.add(spring_length.set("spring length", SPRING_MIN_LENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH));
    physicsParams.add(particleCount.set("PARTICLES", "0"));
    physicsParams.add(springCount.set("SPRINGS", "0"));
    physicsParams.add(attractionCount.set("ATTRACTIONS", "0"));
    physicsParams.add(gravity.set("gravity", ofPoint(0, 0.2, 0), ofPoint(-1, -1, -1), ofPoint(1, 1, 1)));
    physicsParams.add(bindToFixedParticle.set("bind to center", true));
    physicsParams.add(physicsPaused.set("paused", false));

    cameraParams.setName("CAMERA");
    cameraParams.add(orbitCamera.set("Orbit", false));
    cameraParams.add(camFov.set("field of view", 60, 35.f, 180.f));
    cameraParams.add(camNearClip.set("near clip", 0.1f, 0.1f, 20.f));
    cameraParams.add(camFarClip.set("far clip", 5000.f, 20.f, 10000.f));
    
    renderParams.setName("RENDER");
    renderParams.add(drawWireframe.set("wireframe", false));
    renderParams.add(enableLights.set("lights enabled", true));
    renderParams.add(orbitLights.set("Orbit lights", true));
    renderParams.add(lightColor0.set("light 0 color", 1.f, 0.f, 1.f));
    renderParams.add(lightColor1.set("light 1 color", 1.f, 0.f, 1.f));
    renderParams.add(polyColor.set("polygon color", 1.f, 0.f, 1.f));
    renderParams.add(springColor.set("spring color", 1.f, 0.f, 1.f));
    double bs = boxSize.get() * 10.f;
    renderParams.add(lightPos.set("light position", ofPoint(0, 40, -40),
                                  ofPoint(-bs, -bs, -bs),
                                  ofPoint(bs, bs, bs)));
    
    debugParams.setName("DEBUG");
    debugParams.add(drawUsingVboMesh.set("polygons", true));
    debugParams.add(drawGrid.set("grid", true));
    
    gui.add(physicsParams);
    gui.add(cameraParams);
    gui.add(renderParams);
    gui.add(debugParams);
    
    gui.add(useLeap.set("Use Leap", true));
    
    useLeap.addListener(this, &ofApp::toggleLeap);
    
    boxSize.addListener(this, &ofApp::setPhysicsBoxSize);
    gravity.addListener(this, &ofApp::setGravityVec);
    camFov.addListener(this, &ofApp::setCamFov);
    camNearClip.addListener(this, &ofApp::setCamNearClip);
    camFarClip.addListener(this, &ofApp::setCamFarClip);

    bool ul = useLeap.get();
    toggleLeap(ul);
    
    bs = boxSize.get();
    setPhysicsBoxSize(bs);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    float time = ofGetElapsedTimef();
    float bs = boxSize / 2;
    
    if (orbitCamera) {
        float lng = time*10;
        float lat = sin(time*boxSize/1000)*10;
        float radius = sin(time*boxSize/2000)*50 + 600;
        previewCam.orbit(lng, lat, radius);
    }
    
    if (orbitLights) {
        float lat0 = sin(time*0.4)*bs;
        float lng0 = cos(time*0.2)*bs;
        float lat1 = cos(time*0.4)*bs;
        float lng1 = sin(time*0.2)*bs;
        float radius = boxSize;
        pLight0.orbit(lng0, lat0, radius);
        pLight1.orbit(lng1, lat1, radius);
    } else {
        pLight0.setPosition(lightPos);
        pLight1.setPosition(lightPos);
    }
    pLight0.setDiffuseColor(lightColor0);
    pLight1.setDiffuseColor(lightColor1);
    pLight0.setSpecularColor(lightColor0);
    pLight1.setSpecularColor(lightColor1);
    
    
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
    
    particleCount.set(ofToString(physics.numberOfParticles()));
    springCount.set(ofToString(physics.numberOfSprings()));
    attractionCount.set(ofToString(physics.numberOfAttractions()));

    polyMat.setSpecularColor(polyColor);
    polyMat.setDiffuseColor(polyColor);
    springMat.setSpecularColor(springColor);
    springMat.setDiffuseColor(springColor);

    if (!physicsPaused) {
        physics.update();
    }

    if (drawUsingVboMesh) {

        polyMesh.clear();
        polyMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        for(int i=0; i<physics.numberOfParticles(); i++){
            auto p = physics.getParticle(i);
            if (p->isFree()) {
                polyMesh.addVertex(p->getPosition());
                polyMesh.addTexCoord(ofVec2f(p->getPosition()/10));
            }
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
            springMesh.addVertex(b->getPosition());
//            springMesh.addColor(ofFloatColor(1,1,1));
//            springMesh.addColor(ofFloatColor(1,1,1));
//            springMesh.addIndex(i);
//            springMesh.addIndex(i);
        }
    }
}

//--------------------------------------------------------------
void ofApp::loadPreset(){
//    ofFileDialogResult res;
//    res = ofSystemLoadDialog("Load preset");
//    if (res.bSuccess) {
//        gui.loadFromFile(res.filePath);
//    }
    gui.loadFromFile("settings.xml");
}

void ofApp::savePreset(){
//    ofFileDialogResult res;
//    res = ofSystemSaveDialog("settings.xml", "Save preset");
//    if (res.bSuccess) {
//        gui.saveToFile(res.filePath);
//    }
    gui.saveToFile("settings.xml");
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
    spring_length.setMax(s);
    physics.setWorldSize(ofVec3f(-s, -s, -s),
                         ofVec3f(s, s, s));
    physics.clearWorldSize();
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
    if (enableLights) {
        ofEnableLighting();
        pLight0.enable();
        pLight1.enable();
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
        polyTextureImage.bind();
        if (drawWireframe)  polyMesh.drawWireframe();
        else                polyMesh.draw();
        polyTextureImage.unbind();
        polyMat.end();
        
        springMat.begin();
        if (drawWireframe)  springMesh.drawWireframe();
        else                springMesh.draw();
        springMat.end();
        
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
    
    if (drawGrid) {
        ofSetColor(ofColor::white, 255);
    }
    
    if (enableLights) {
//        ofSetColor(lightColor0->getClamped());
//        pLight0.draw();
//        ofSetColor(lightColor1->getClamped());
//        pLight1.draw();
        ofDisableLighting();
    }
    previewCam.end();
    ofPopMatrix();
    
    ofDisableAlphaBlending();
    ofDisableDepthTest();
    
    if (drawGui) {
        ofSetColor(ofColor::white);
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
        case 'C':
            makeCluster();
            break;
        case ' ':
      {
        physics.clear();
        physics.addParticle(&fixedParticle);
//        auto z = ofPoint::zero();
//        setGravityVec(z);
        break;
      }
        case 'S':
            savePreset();
            break;
        case 'L':
            loadPreset();
            break;
        case 'F':
            ofToggleFullscreen();
            break;
        case 'G':
            drawGui.set(!drawGui.get());
        case 'M':
            gui.minimizeAll();
            break;
        case 'N':
            gui.maximizeAll();
            break;
        case 'Q':
            makeParticles = !makeParticles;
            break;
        case 'W':
            makeSprings = !makeSprings;
            break;
        case '/':
            randomiseParams();
            break;
        case '.':
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
    if (!drawGui || (drawGui && !gui.getShape().inside(x, y))){
        if (makeParticles) {
//            makeCluster();
//            mousePressed(x, y, button);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (!gui.getShape().inside(x, y)){
//        makeCluster();
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
    gui.setSize(w/4, h);
    gui.setWidthElements(w/4);
    gui.setDefaultWidth(w/4);
    gui.setDefaultHeight(20);
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
    int np = physics.numberOfParticles();
    
    if (makeParticles) {
        //            auto pos = previewCam.screenToWorld(ofVec3f(x, y, 0));
        //            makeParticleAtPosition(pos);
        makeParticleAtCenter(boxSize.get() * 0.8f);
    }
    if (makeSprings && np > 1 && np % 2 == 0) {
        auto a = physics.getParticle(np-1);
        auto b = physics.getParticle(np-2);
        
        if (bindToFixedParticle) {
            makeSpringBetweenParticles(a, &fixedParticle);
            makeSpringBetweenParticles(b, &fixedParticle);
        } else {
            
        }
        auto r = physics.getParticle((int)ofRandom(np-1));
        makeSpringBetweenParticles(a, r);
        physics.makeAttraction(a, b, attraction);
    }
}