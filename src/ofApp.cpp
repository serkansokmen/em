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

    polyMat.setShininess(255);
    polyMat.setDiffuseColor(ofFloatColor(0,1,0));

    springMat.setShininess(255);
    springMat.setDiffuseColor(ofFloatColor(1,1,1));

    lightColor.setBrightness( 250.f );
    lightColor.setSaturation( 150.f );

    materialColor.setBrightness(250.f);
    materialColor.setSaturation(200);

    setupGui();
    
    float boxSize = 100.f;
    physics.setWorldSize(ofVec3f(-boxSize, -boxSize, -boxSize),
                         ofVec3f(boxSize, boxSize, boxSize));
    physics.setSectorCount(SECTOR_COUNT);
    physics.setDrag(0.97f);
    // physics.setTimeStep(60);
    physics.setDrag(0.97f);
    physics.setDrag(1);
    physics.disableCollision();
    
    ofxLoadCamera(previewCamera, "previewCamSettings");
    previewCamera.setFov(60);
}

//--------------------------------------------------------------
void ofApp::setupGui(){

    gui.setup();

    ofParameterGroup    physicsParams;
    ofParameterGroup    renderParams;
    ofParameterGroup    cameraParams;

    physicsParams.setName("PHYSICS");
    physicsParams.add(makeParticles.set("Particles", true));
    physicsParams.add(makeSprings.set("Springs", true));
    physicsParams.add(radius.set("Radius", NODE_MIN_RADIUS, NODE_MIN_RADIUS, NODE_MAX_RADIUS));
    physicsParams.add(mass.set("Mass", MIN_MASS, MIN_MASS, MAX_MASS));
    physicsParams.add(bounce.set("Bounce", MIN_BOUNCE, MIN_BOUNCE, MAX_BOUNCE));
    physicsParams.add(attraction.set("Attraction", MIN_ATTRACTION, MIN_ATTRACTION, MAX_ATTRACTION));
    physicsParams.add(spring_strength.set("Spring Strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    physicsParams.add(spring_length.set("Spring Length", SPRING_MIN_LENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH));
    physicsParams.add(springCount.set("SPRING COUNT", "0"));
    physicsParams.add(gravity.set("Gravity", ofPoint(0, 0.2, 0), ofPoint(-1, -1, -1), ofPoint(1, 1, 1)));
    physicsParams.add(physicsPaused.set("Paused", false));

    cameraParams.setName("CAMERA");
    cameraParams.add(camNearClip.set("Near Clip", previewCamera.getNearClip(), 0.1f, 200.f));
    cameraParams.add(camFarClip.set("Far Clip", previewCamera.getFarClip(), 200.f, 4000.f));
    
    renderParams.setName("RENDER");
    renderParams.add(drawUsingVboMesh.set("Draw VBO", true));
    renderParams.add(colorHue.set("Color Hue", 100.f, 0.f, 255.f));
    renderParams.add(lightPos.set("Light Position", ofPoint(0, 40, -40),
                                  ofPoint(-ofGetWidth()*2, -ofGetHeight()*2, -ofGetWidth()),
                                  ofPoint(ofGetWidth()*2, ofGetHeight()*2, ofGetWidth())));

    gui.add(physicsParams);
    gui.add(cameraParams);
    gui.add(renderParams);

    gui.setBackgroundColor(ofColor(28,28,28));
    gui.add(drawGui.set("GUI", true));
    
    gravity.addListener(this, &ofApp::setGravity);
    camNearClip.addListener(this, &ofApp::setCamNearClip);
    camFarClip.addListener(this, &ofApp::setCamFarClip);

    gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){

    springCount.set(ofToString(physics.numberOfSprings()));

    lightColor.setHue(colorHue);
    pointLight.setPosition(lightPos);
    pointLight.setDiffuseColor(lightColor);
    materialColor.setHue(colorHue);
    polyMat.setSpecularColor(materialColor);

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
            auto s = physics.getSpring(i);
            auto a = s->getOneEnd();
            auto b = s->getTheOtherEnd();
            auto vec = b->getPosition() - a->getPosition();
            float dist = vec.normalize().length();

            springMesh.addVertex(a->getPosition());
            springMesh.addVertex(b->getPosition());
            springMesh.addIndex(i);
            springMesh.addIndex(i);
            polyMesh.addIndex(i);
        }
    }
}

//--------------------------------------------------------------
void ofApp::setGravity(ofPoint& g){
    physics.setGravity(g);
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

    if (drawGui) {
        ofSetColor(255, 200);
        ofDrawAxis(1);
        ofDrawGridPlane(1);
    }

    if (drawUsingVboMesh) {
        ofPushMatrix();
        polyMat.begin();
        polyMesh.drawFaces();
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
    gravity.removeListener(this, &ofApp::setGravity);
    camNearClip.removeListener(this, &ofApp::setCamNearClip);
    camFarClip.removeListener(this, &ofApp::setCamFarClip);
    ofxSaveCamera(previewCamera, "previewCamSettings");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case ' ':
            physics.clear();
            break;
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
            drawGui = !drawGui;
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
        ofColor color(ofNormalize(mass, MIN_MASS, MAX_MASS)*255,
                ofNormalize(radius, NODE_MIN_RADIUS, NODE_MAX_RADIUS)*255,
                ofNormalize(bounce, MIN_BOUNCE, MAX_BOUNCE)*255);
        if (makeParticles) {
            auto pos = previewCamera.worldToCamera(ofVec3f(x, y, 0));
            makeParticleAtPosition(pos, color);
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
    physics.clearWorldSize();
    physics.setWorldSize(ofPoint(-w, -h, -h), ofPoint(w, h, h));
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
void ofApp::makeParticleAtPosition(const ofPoint& p, const ofColor& c){
    auto a = new Particle3D;
    a->setMass(mass)
    ->setBounce(bounce)
    ->setRadius(radius)
    ->enableCollision()
    ->makeFree()
    ->moveTo(p);

    physics.addParticle(a);

    if (makeSprings) {
        for (int i = 0; i < physics.numberOfParticles(); i++) {
            float dist = physics.getParticle(i)->getPosition().distance(a->getPosition());
            if (dist > SPRING_MIN_LENGTH && dist < spring_length) {
                makeSpringBetweenParticles(a, physics.getParticle(i));
            }
        }
    }
    for (int i = 0; i < physics.numberOfParticles(); i++) {
        float dist = physics.getParticle(i)->getPosition().distance(a->getPosition());
        if (dist > SPRING_MIN_LENGTH && dist < spring_length) {
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
