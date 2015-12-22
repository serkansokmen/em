#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofSetSmoothLighting(true);
    
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
    
//    physics.verbose = true;			// dump activity to log
#ifdef USE_3D
    physics.setWorldSize(ofVec3f(-ofGetWidth()/2, -ofGetHeight(), -ofGetWidth()/2), ofVec3f(ofGetWidth()/2, ofGetHeight(), ofGetWidth()/2));
#else
    physics.setWorldSize(ofVec2f(0, 0), ofVec2f(ofGetWidth(), ofGetHeight()));
#endif
    physics.setSectorCount(SECTOR_COUNT);
    physics.setDrag(0.97f);
    physics.setTimeStep(60);
//    physics.setDrag(0.97f);
    physics.setDrag(1);
    physics.disableCollision();
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    gui.setup();
    gui.add(makeParticles.set("Particles", true));
    gui.add(makeSprings.set("Springs", true));
    gui.add(gravity.set("gravity", ofVec2f(0, 0.2), ofVec2f(-1, -1), ofVec2f(1, 1)));
    particleParams.setName("particles");
    particleParams.add(node_radius.set("node radius", NODE_MIN_RADIUS, NODE_MIN_RADIUS, NODE_MAX_RADIUS));
    particleParams.add(mass.set("mass", MIN_MASS, MIN_MASS, MAX_MASS));
    particleParams.add(bounce.set("bounce", MIN_BOUNCE, MIN_BOUNCE, MAX_BOUNCE));
    particleParams.add(attraction.set("attraction", MIN_ATTRACTION, MIN_ATTRACTION, MAX_ATTRACTION));
    
    springParams.setName("springs");
    springParams.add(spring_strength.set("spring strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    springParams.add(spring_length.set("spring length", SPRING_MIN_LENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH));
    
    gui.add(lightPos.set("Light position", ofPoint::zero(), ofVec3f(-ofGetWidth(), -ofGetHeight(), -ofGetWidth()), ofVec3f(ofGetWidth(), ofGetHeight(), ofGetWidth())));
    gui.add(colorHue.set("HUE", 100.f, 0.f, 255.f));
    
    gui.add(particleParams);
    gui.add(springParams);
    gui.add(drawGui.set("GUI", true));
    gui.add(physicsPaused.set("Pause", false));
    gui.add(drawUsingVboMesh.set("VBO Mesh", true));
    
//    gui.minimizeAll();
    gui.loadFromFile("settings.xml");
    
    gravity.addListener(this, &ofApp::setGravity);
}

//--------------------------------------------------------------
void ofApp::update(){
    
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
        polygonMesh.clear();
        polygonMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY);
        for(int i=0; i<physics.numberOfParticles(); i++){
            auto p = (Particle *)physics.getParticle(i);
            polygonMesh.addVertex(p->getPosition());
            polygonMesh.addColor(ofFloatColor(p->color.a/255, p->color.g/255, p->color.b/255));
            polygonMesh.addIndex(i);
        }
        
        springMesh.clear();
        springMesh.setMode(OF_PRIMITIVE_LINES);
        for(int i=0; i<physics.numberOfSprings(); i++){
            auto s = (Spring2D *)physics.getSpring(i);
            auto a = (Particle *)s->getOneEnd();
            auto b = (Particle *)s->getTheOtherEnd();
            ofVec2f vec = b->getPosition() - a->getPosition();
            float dist = vec.normalize().length();
            ofFloatColor c;
            c.r = ofLerp(a->color.r, b->color.r, dist)/255;
            c.g = ofLerp(a->color.g, b->color.g, dist)/255;
            c.b = ofLerp(a->color.b, b->color.b, dist)/255;
            springMesh.addVertex(a->getPosition());
            springMesh.addVertex(b->getPosition());
            polygonMesh.addColor(c);
            polygonMesh.addColor(c);
            polygonMesh.addIndex(i);
            if (i == physics.numberOfSprings() - 1) {
                polygonMesh.addIndex(i);
            } else {
                polygonMesh.addIndex(i+1);
            }
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::setGravity(ofVec2f& g){
    physics.setGravity(g);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    float width = ofGetWidth();
    float height = ofGetHeight();
    
    ofPushMatrix();
//    ofEnableDepthTest();
    ofEnableAlphaBlending();
    
    arcball.begin();
    
    if (drawUsingVboMesh) {
        ofEnableLighting();
        pointLight.enable();

        polyMat.begin();
        polygonMesh.drawFaces();
        polyMat.end();
        
        springMat.begin();
        springMesh.draw();
        springMat.end();

        ofDisableLighting();
    } else {
        // Draw springs
        for(int i=0; i<physics.numberOfSprings(); i++){
            auto s = (Spring2D *)physics.getSpring(i);
            auto a = (Particle *)s->getOneEnd();
            auto b = (Particle *)s->getTheOtherEnd();
            ofVec2f vec = b->getPosition() - a->getPosition();
            float dist = vec.normalize().length();
            ofColor c;
            c.r = ofLerp(a->color.r, b->color.r, dist);
            c.g = ofLerp(a->color.g, b->color.g, dist);
            c.b = ofLerp(a->color.b, b->color.b, dist);
            ofSetColor(c);
            ofDrawLine(a->getPosition(), b->getPosition());
        }
        
        // Draw particles
        for(int i=0; i<physics.numberOfParticles(); i++){
            auto p = (Particle *)physics.getParticle(i);
            ofSetColor(p->color);
            ofSetCircleResolution(p->getRadius()*10);
            ofDrawCircle(p->getPosition(), p->getRadius());
        }
    }
    
    ofDisableAlphaBlending();
//    ofDisableDepthTest();
    ofPopMatrix();
    
    arcball.end();
    
    ofSetColor(ofColor::white);
    if (drawGui) {
        gui.draw();
    }
}

void ofApp::exit(){
    gravity.removeListener(this, &ofApp::setGravity);
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
        case 'g':
            drawGui = !drawGui;
        case 'b':
            for(int i=0; i<physics.numberOfAttractions(); i++){
                auto a = (Attraction2D *)physics.getAttraction(i);
                a->kill();
//                physics.makeAttraction(, <#ParticleT<ofVec2f> *b#>, <#float _strength#>);
            }
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
        mousePressed(x, y, button);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (!gui.getShape().inside(x, y)){
        ofColor color(ofNormalize(mass, MIN_MASS, MAX_MASS)*255,
                ofNormalize(node_radius, NODE_MIN_RADIUS, NODE_MAX_RADIUS)*255,
                ofNormalize(bounce, MIN_BOUNCE, MAX_BOUNCE)*255);
        makeParticleAtPosition(x, y, color);
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
    physics.setWorldSize(ofVec2f(0, 0), ofVec2f(w, h));
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
void ofApp::makeParticleAtPosition(float x, float y, ofColor c){
    auto a = new Particle;
    a->moveTo(ofVec2f(x, y));
    a->color.set(c);
    a->setMass(mass)
    ->setBounce(bounce)
    ->setRadius(node_radius)
    ->enableCollision()
    ->makeFree();
    
    if (makeParticles)
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
template <typename T>
void ofApp::makeSpringBetweenParticles(ParticleT<T> *a, ParticleT<T> *b) {
    float dist = a->getPosition().distance(b->getPosition());
    float strength = dist*spring_strength;
    physics.makeSpring(a, b, spring_strength, spring_length);
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
            float radius = size * node_radius;
            
            if (ofGetWindowRect().inside(pos)) {
                
                auto a = new Particle;
                a->color = ofColor(cur);
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