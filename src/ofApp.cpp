#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    setupGui();
    
//    physics.verbose = true;			// dump activity to log
    physics.setWorldSize(ofVec2f(0, 0), ofVec2f(ofGetWidth(), ofGetHeight()));
    physics.setSectorCount(SECTOR_COUNT);
    physics.setDrag(0.97f);
    physics.setTimeStep(10);
//    physics.setDrag(1);
    physics.enableCollision();
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    gui.setup();
    gui.add(gravity.set("gravity", ofVec2f(0, 0.2), ofVec2f(-1, -1), ofVec2f(1, 1)));
    particleParams.setName("particles");
    particleParams.add(node_radius.set("node radius", NODE_MIN_RADIUS, NODE_MIN_RADIUS, NODE_MAX_RADIUS));
    particleParams.add(mass.set("mass", MIN_MASS, MIN_MASS, MAX_MASS));
    particleParams.add(bounce.set("bounce", MIN_BOUNCE, MIN_BOUNCE, MAX_BOUNCE));
    particleParams.add(attraction.set("attraction", MIN_ATTRACTION, MIN_ATTRACTION, MAX_ATTRACTION));
    
    springParams.setName("springs");
    springParams.add(spring_strength.set("spring strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    springParams.add(spring_length.set("spring length", SPRING_MIN_LENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH));
    
    gui.add(particleParams);
    gui.add(springParams);
    gui.minimizeAll();
    gui.loadFromFile("settings.xml");
    
    gravity.addListener(this, &ofApp::setGravity);
}

//--------------------------------------------------------------
void ofApp::update(){
    physics.update();
}

void ofApp::setGravity(ofVec2f& g){
    physics.setGravity(g);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // Draw springs
    ofPushMatrix();
    ofEnableDepthTest();
    ofEnableAlphaBlending();
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
//        ofSetColor(c);
        ofSetColor(ofColor::white, 100);
        ofDrawLine(a->getPosition(), b->getPosition());
    }
    ofDisableAlphaBlending();
    ofDisableDepthTest();
    ofPopMatrix();
    
    // Draw particles
    ofPushMatrix();
    ofEnableDepthTest();
    for(int i=0; i<physics.numberOfParticles(); i++){
        auto p = (Particle *)physics.getParticle(i);
        ofSetColor(p->color);
        ofSetCircleResolution(p->getRadius()*10);
        ofDrawCircle(p->getPosition(), p->getRadius());
    }
    ofDisableDepthTest();
    ofPopMatrix();
    
    gui.draw();
    
    ofSetColor(ofColor::white);
    ofDrawBitmapString("Particles: " + ofToString(physics.numberOfParticles()), 400, 40);
    ofDrawBitmapString("Springs: " + ofToString(physics.numberOfSprings()), 400, 60);
    ofDrawBitmapString("Attractions: " + ofToString(physics.numberOfAttractions()), 400, 80);
    ofDrawBitmapString("Constraints: " + ofToString(physics.numberOfConstraints()), 400, 100);
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
    if (!gui.getShape().inside(x, y)){
        makeParticleAtPosition(x, y, ofColor::white);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (!gui.getShape().inside(x, y)){
        makeParticleAtPosition(x, y, ofColor::white);
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
        if (img.load(info.files[0])) {
            makeParticlesFromImage(img);
        }
    }
}

//--------------------------------------------------------------
void ofApp::makeParticleAtPosition(float x, float y, ofColor c){
    
    float radius	 = ofMap(mass, MIN_MASS, MAX_MASS, NODE_MIN_RADIUS, NODE_MAX_RADIUS);
    
    auto a = new Particle;
    a->moveTo(ofVec2f(x, y));
    a->color.set(c);
    a->setMass(mass)
    ->setBounce(bounce)
    ->setRadius(radius)
    ->enableCollision()
    ->makeFree();
    
    physics.addParticle(a);
    
    for (int i=0; i<physics.numberOfParticles(); i++) {
        if (i > 0) {
            
            auto b = physics.getParticle(i-1);
            float dist = a->getPosition().distance(b->getPosition());
            if (dist > SPRING_MIN_LENGTH && dist < spring_length) {
                float strength = dist*spring_strength*0.01;
                physics.makeSpring(a, b, spring_strength, spring_length);
            }
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
                    if (bAddSpring) {
                        physics.makeSpring(a, b, spring_strength, spring_length);
                    }
                    physics.makeAttraction(a, b, attraction);
                }
                
                a->release();
            }
        }
    }
}