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

    pointLight.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
    pointLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));
    
    ofColor c;
    c.setBrightness(250.f);
    c.setSaturation(200.f);
    lightColor.set(c);

    polyMatDiffuseColor.set(0,1,0);
    springMatDiffuseColor.set(1,1,1);
    
    polyMat.setShininess(255);
    polyMat.setDiffuseColor(polyMatDiffuseColor);
    springMat.setShininess(255);
    springMat.setDiffuseColor(springMatDiffuseColor);

    setupGui();
    
    physics.setSectorCount(SECTOR_COUNT);
    physics.setDrag(0.97f);
    // physics.setTimeStep(60);
    physics.setDrag(0.97f);
    physics.setDrag(1);
    physics.enableCollision();
    
    physics.addParticle(&fixedParticle);
    fixedParticle.setMass(1)->setRadius(10.f)->moveTo(ofPoint::zero())->makeFixed();
    
    ofxLoadCamera(previewCamera, "previewCamSettings");
    previewCamera.setFov(camFov);
}

//--------------------------------------------------------------
void ofApp::setupGui(){

    gui.setup();
    drawGui.set(true);
    
    ofParameterGroup    physicsParams;
    ofParameterGroup    renderParams;
    ofParameterGroup    debugParams;
    ofParameterGroup    cameraParams;
    
    physicsParams.setName("PHYSICS");
    physicsParams.add(boxSize.set("world size", 100.f, 1.f, 400.f));
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
    physicsParams.add(physicsPaused.set("paused", false));

    cameraParams.setName("CAMERA");
    cameraParams.add(camFov.set("field of view", 60, 0.f, 180.f));
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
    debugParams.add(drawUsingVboMesh.set("polygons", true));
    debugParams.add(drawGrid.set("grid", true));
    debugParams.add(drawWorldBox.set("world box", true));
    
    gui.add(physicsParams);
    gui.add(cameraParams);
    gui.add(renderParams);
    gui.add(debugParams);
    
    gui.minimizeAll();
    

    gui.setBackgroundColor(ofColor::black);
    
    gui.setBorderColor(ofColor::black);
    gui.setDefaultTextPadding(10);
    
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

    particleCount.set(ofToString(physics.numberOfParticles()));
    springCount.set(ofToString(physics.numberOfSprings()));
    attractionCount.set(ofToString(physics.numberOfAttractions()));
    
    lightColor.setHue(lightHue);
    pointLight.setPosition(lightPos);
    pointLight.setSpecularColor(lightColor);
    
    polyMatDiffuseColor.setHue(polyHue);
    springMatDiffuseColor.setHue(springHue);

    polyMat.setDiffuseColor(polyMatDiffuseColor);
    springMat.setDiffuseColor(springMatDiffuseColor);

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
            polyMesh.addVertex(p->getPosition());
            polyMesh.addIndex(i);
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
            springMesh.addVertex(b->getPosition());
            springMesh.addIndex(i);
            springMesh.addIndex(i);
//            polyMesh.addIndex(i);
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
    previewCamera.setFov(v);
}
void ofApp::setCamNearClip(float& v) {
    previewCamera.setNearClip(v);
}
void ofApp::setCamFarClip(float& v) {
    previewCamera.setFarClip(v);
}

void ofApp::resetCamera(){
    previewCamera.setDistance(3.0f);
    previewCamera.setNearClip(0.01f);
    previewCamera.setFarClip(500.0f);
    previewCamera.setPosition(0.4f, 0.2f, 0.8f);
    previewCamera.lookAt(ofVec3f(0.0f, 0.0f, 0.0f));
}

//--------------------------------------------------------------
void ofApp::draw(){

    float width = ofGetWidth();
    float height = ofGetHeight();

    ofPushMatrix();
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableLighting();
    pointLight.enable();
    previewCamera.begin();
    
    if (drawWorldBox) {
        worldBox.drawWireframe();
    }
    
    if (drawGrid) {
        ofSetColor(255, 200);
        ofDrawAxis(10);
//        ofDrawGridPlane(100);
        ofDrawGrid(40);
    }

    if (drawUsingVboMesh) {
        ofPushMatrix();
        polyMat.begin();
        polyMesh.draw();
        polyMat.end();

        springMat.begin();
        springMesh.draw();
        springMat.end();

        ofPopMatrix();
        
    } else {
        for(int i=0; i<physics.numberOfParticles(); i++){
            auto p = physics.getParticle(i);
            ofPushMatrix();
            ofTranslate(p->getPosition());
            ofSetColor(200);
            ofSetCircleResolution(p->getRadius()*10);
            polyMat.begin();
            ofDrawSphere(p->getRadius());
            polyMat.end();
            ofPopMatrix();
        }
        
        // draw springs
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
    
    if (drawGui) {
        ofSetColor(ofColor::white);
        pointLight.draw();
    }
    ofDisableLighting();
    previewCamera.end();
    ofDisableAlphaBlending();
    ofDisableDepthTest();
    ofPopMatrix();

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
    ofxSaveCamera(previewCamera, "previewCamSettings");
    gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
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
            mousePressed(x, y, button);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (!gui.getShape().inside(x, y)){
        
        int np = physics.numberOfParticles();
        
        if (makeParticles) {
//            auto pos = previewCamera.screenToWorld(ofVec3f(x, y, 0));
//            makeParticleAtPosition(pos);
            makeParticleAtCenter(boxSize.get() * 0.8f);
        }
        if (makeSprings && np > 1 && np % 2 == 0) {
            auto a = physics.getParticle(np-1);
            auto b = physics.getParticle(np-2);
            makeSpringBetweenParticles(a, b);
            makeSpringBetweenParticles(a, &fixedParticle);
            makeSpringBetweenParticles(b, &fixedParticle);
        }
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
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo info){
    physics.clear();
    if (info.files.size() > 0) {
        ofImage img;
        if (img.load(info.files[0]) && makeParticles) {
            makeParticlesFromImage(img);
        }
    }
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
void ofApp::makeParticlesFromImage(ofImage &img){

    img.crop(0, 0, ofGetHeight(), ofGetHeight());

    int w = img.getWidth();
    int h = img.getHeight();
    int step = 20;

    for(int y = 0; y < h; y+=step) {
        for(int x = 0; x < w; x+=step) {

            ofVec2f pos(x, y);
            ofColor cur = img.getColor(x, y);
            float size = 1 - (cur.getBrightness() / 255);
            float radius = size * radius;

            if (ofGetWindowRect().inside(pos)) {

                auto a = new Particle3D;
                a->setMass(mass)
                ->setBounce(bounce)
                ->setRadius(radius)
                ->enableCollision()
                ->makeFree()->moveTo(pos);

                physics.addParticle(a);

                if (physics.numberOfParticles() > 1) {
                    auto b = physics.getParticle(physics.numberOfParticles()-2);
                    bool bAddSpring = ofRandom(-100, 100) > 0;
                    if (bAddSpring && makeSprings) {
                        physics.makeSpring(a, b, spring_strength, spring_length);
                    }
                    physics.makeAttraction(a, b, attraction);
                }

                a->release();
            }
        }
    }
}
