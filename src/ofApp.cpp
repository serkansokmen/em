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

    ofColor c;
    c.setBrightness(250.f);
    c.setSaturation(200.f);
    lightColor.set(c);
    
    pointLight.setDiffuseColor(lightColor.getHue());
    pointLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));

    polyMatDiffuseColor.set(0,1,0);
    springMatDiffuseColor.set(1,1,1);
    
    polyMat.setShininess(255);
    springMat.setShininess(255);
    polyMat.setDiffuseColor(polyMatDiffuseColor);
    springMat.setDiffuseColor(springMatDiffuseColor);
    
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
    physicsParams.add(radius.set("particle radius", NODE_MIN_RADIUS, NODE_MIN_RADIUS, NODE_MAX_RADIUS));
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
    cameraParams.add(camFov.set("field of view", 60, 35.f, 180.f));
    cameraParams.add(camNearClip.set("near clip", 0.1f, 0.1f, 20.f));
    cameraParams.add(camFarClip.set("far clip", 5000.f, 20.f, 10000.f));
    
    renderParams.setName("RENDER");
    renderParams.add(lightHue.set("light hue", 100.f, 0.f, 255.f));
    renderParams.add(polyHue.set("polygon hue", 100.f, 0.f, 255.f));
    renderParams.add(springHue.set("spring hue", 255.f, 0.f, 255.f));
    double bs = boxSize.get() * 1.8f;
    renderParams.add(lightPos.set("light position", ofPoint(0, 40, -40),
                                  ofPoint(-bs, -bs, -bs),
                                  ofPoint(bs, bs, bs)));
    debugParams.setName("DEBUG");
    debugParams.add(drawWireframe.set("wireframe", false));
    debugParams.add(drawUsingVboMesh.set("polygons", true));
    debugParams.add(drawGrid.set("grid", true));
    debugParams.add(drawWorldBox.set("world box", true));
    
    gui.add(physicsParams);
    gui.add(cameraParams);
    gui.add(renderParams);
    gui.add(debugParams);
    
    boxSize.addListener(this, &ofApp::setPhysicsBoxSize);
    gravity.addListener(this, &ofApp::setGravityVec);
    camFov.addListener(this, &ofApp::setCamFov);
    camNearClip.addListener(this, &ofApp::setCamNearClip);
    camFarClip.addListener(this, &ofApp::setCamFarClip);

    gui.loadFromFile("settings.xml");
    
    bs = boxSize.get();
    setPhysicsBoxSize(bs);
}

//--------------------------------------------------------------
void ofApp::update(){
    
//    previewCam.orbit(ofGetElapsedTimef() * 3.f * 10.f, 0.2f, previewCam.getDistance());
    
    particleCount.set(ofToString(physics.numberOfParticles()));
    springCount.set(ofToString(physics.numberOfSprings()));
    attractionCount.set(ofToString(physics.numberOfAttractions()));
    
    lightColor.setHue(lightHue);
    pointLight.setPosition(lightPos);
    pointLight.setDiffuseColor(lightColor);
    
    polyMatDiffuseColor.setHue(polyHue);
    springMatDiffuseColor.setHue(springHue);

    polyMat.setSpecularColor(polyMatDiffuseColor);
    springMat.setSpecularColor(springMatDiffuseColor);

    if (!physicsPaused) {
        physics.update();
    }

    // Remove distant attractions
    for(int i=0; i<physics.numberOfAttractions(); i++){
        auto a = (Attraction2D *)physics.getAttraction(i);
        auto p0 = a->getOneEnd();
        auto p1 = a->getTheOtherEnd();

        if (p0->getPosition().distance(p1->getPosition()) > MAX_DISTANCE) {
            a->kill();
        }
    }

    if (drawUsingVboMesh) {

        polyMesh.clear();
        polyMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY);
        for(int i=0; i<physics.numberOfParticles(); i++){
            auto p = physics.getParticle(i);
            if (p->isFree()) {
                polyMesh.addVertex(p->getPosition());
                polyMesh.addColor(ofColor::fromHsb(ofRandom(255), 255, 255));
                polyMesh.addIndex(i);
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
            
//            ofRotate(angle, rx, ry, 0.0);
            float size = ofMap(spring->getStrength(), SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH);

            springMesh.addVertex(a->getPosition());
            polyMesh.addColor(ofColor::fromHsb(0, 255, 0));
            springMesh.addVertex(b->getPosition());
            polyMesh.addColor(ofColor::fromHsb(0, 255, 0));
            springMesh.addIndex(i);
            springMesh.addIndex(i);
        }
    }
}

//--------------------------------------------------------------
void ofApp::setPhysicsBoxSize(double& s){
    worldBox.setScale(1);
    worldBox.setWidth(s*2);
    worldBox.setHeight(s*2);
    worldBox.setDepth(s*2);
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
    previewCam.setDistance(5.0f);
    previewCam.setNearClip(0.01f);
    previewCam.setFarClip(5000.0f);
    previewCam.setPosition(0.4f, 0.2f, 0.8f);
    previewCam.lookAt(ofVec3f(0.0f, 0.0f, 0.0f));
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    float width = ofGetWidth();
    float height = ofGetHeight();
    
    ofPushMatrix();
    ofEnableLighting();
    
    previewCam.begin();
    pointLight.enable();
    
    
    if (drawWorldBox) {
        worldBox.drawWireframe();
    }
    
    if (drawGrid) {
        ofSetColor(255, 50);
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
        
        // Draw spring mesh
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
            
            ofPushMatrix();
            springMat.begin();
            ofDrawLine(a->getPosition(), b->getPosition());
            springMat.end();
            ofPopMatrix();
        }
    }
    
    previewCam.end();
    ofDisableLighting();
    ofPopMatrix();
    
    ofDisableAlphaBlending();
    ofDisableDepthTest();
    
    if (drawGui) {
        ofSetColor(ofColor::white);
        gui.draw();
    }
}

void ofApp::exit(){
    boxSize.removeListener(this, &ofApp::setPhysicsBoxSize);
    gravity.removeListener(this, &ofApp::setGravityVec);
    camFov.removeListener(this, &ofApp::setCamFov);
    camNearClip.removeListener(this, &ofApp::setCamNearClip);
    camFarClip.removeListener(this, &ofApp::setCamFarClip);
    ofxSaveCamera(previewCam, "preview_cam_settings");
    gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'c':
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
        case 's':
            gui.saveToFile("settings.xml");
            break;
        case 'l':
            gui.loadFromFile("settings.xml");
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case '.':
            resetCamera();
            break;
        case 'g':
            drawGui.set(!drawGui.get());
        case 'm':
            gui.minimizeAll();
            break;
        case 'n':
            gui.maximizeAll();
            break;
        case 'b':
            for(int i=0; i<physics.numberOfAttractions(); i++){
                auto a = (Attraction2D *)physics.getAttraction(i);
                a->kill();
            }
        case 'q':
            makeParticles = !makeParticles;
            break;
        case 'w':
            makeSprings = !makeSprings;
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

    for (int i = 0; i < physics.numberOfParticles(); i++) {
        float dist = physics.getParticle(i)->getPosition().distance(a->getPosition());
        if (dist > SPRING_MIN_LENGTH && dist < spring_length && attraction > 0.f) {
            physics.makeAttraction(a, physics.getParticle(i), attraction);
        }
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
        makeSpringBetweenParticles(a, b);
        if (bindToFixedParticle) {
            makeSpringBetweenParticles(a, &fixedParticle);
            makeSpringBetweenParticles(b, &fixedParticle);
        }
    }
}