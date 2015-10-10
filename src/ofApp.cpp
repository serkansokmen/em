#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(255);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    setupGui();
    
    physics.verbose = true;			// dump activity to log
    
    // set world dimensions, not essential, but speeds up collision
    physics.setWorldSize(ofVec2f(0, 0), ofVec2f(ofGetWidth(), ofGetHeight()));
    physics.setSectorCount(SECTOR_COUNT);
    physics.setDrag(0.97f);
//    physics.setDrag(1);
    physics.enableCollision();
    
    // init scene
    physics.clear();
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    gui.setup();
    gui.add(gravity.set("gravity", ofVec2f(0, 0.2), ofVec2f::zero(), ofVec2f(1, 1)));
    particleParams.setName("particles");
    particleParams.add(node_min_radius.set("node min radius", NODE_MIN_RADIUS, NODE_MIN_RADIUS, NODE_MAX_RADIUS));
    particleParams.add(node_max_radius.set("node max radius", NODE_MAX_RADIUS, NODE_MIN_RADIUS, NODE_MAX_RADIUS));
    particleParams.add(min_mass.set("min mass", MIN_MASS, MIN_MASS, MAX_MASS));
    particleParams.add(max_mass.set("max mass", MAX_MASS, MIN_MASS, MAX_MASS));
    particleParams.add(min_bounce.set("min bounce", MIN_BOUNCE, MIN_BOUNCE, MAX_BOUNCE));
    particleParams.add(max_bounce.set("max bounce", MAX_BOUNCE, MIN_BOUNCE, MAX_BOUNCE));
    particleParams.add(min_attraction.set("min attraction", MIN_ATTRACTION, MIN_ATTRACTION, MAX_ATTRACTION));
    particleParams.add(max_attraction.set("max attraction", MAX_ATTRACTION, MIN_ATTRACTION, MAX_ATTRACTION));
    
    springParams.setName("springs");
    springParams.add(spring_min_strength.set("spring min strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    springParams.add(spring_max_strength.set("spring max strength", SPRING_MAX_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
    springParams.add(spring_min_width.set("spring min width", SPRING_MIN_WIDTH, SPRING_MIN_WIDTH, SPRING_MAX_WIDTH));
    springParams.add(spring_max_width.set("spring max width", SPRING_MAX_WIDTH, SPRING_MIN_WIDTH, SPRING_MAX_WIDTH));
    
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
    
    ofPushMatrix();
    ofEnableDepthTest();
    for(int i=0; i<physics.numberOfParticles(); i++){
        auto p = (Particle *)physics.getParticle(i);
        ofSetColor(p->color); // @TODO: set individual particle color
        ofSetCircleResolution(p->getRadius()*10);
        ofDrawCircle(p->getPosition(), p->getRadius());
    }
    ofDisableDepthTest();
    ofPopMatrix();
    
    gui.draw();
    
    ofSetColor(ofColor::black);
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
    makeParticleAtPosition(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    makeParticleAtPosition(x, y);
}

//--------------------------------------------------------------
void ofApp::makeParticleAtPosition(float x, float y){
    
    float mass = ofRandom(min_mass, max_mass);
    float bounce = ofRandom(min_bounce, max_bounce);
    float radius	 = ofMap(mass, min_mass, max_mass, node_min_radius, node_max_radius);
    auto a = new Particle;
    
    a->moveTo(ofVec2f(x, y));
    a->color = ofColor(ofMap(radius, node_min_radius, node_max_radius, 0, 255));
    
    physics.addParticle(a);
    
    if (physics.numberOfParticles() > 1) {
        auto b = physics.getParticle(physics.numberOfParticles() - 1);
        physics.makeSpring(a, b, ofRandom(spring_min_strength, spring_max_strength), ofRandom(10, ofGetWidth()/2));
        physics.makeAttraction(b, a, ofRandom(min_attraction, max_attraction));
    }
    
    a->setMass(mass)
    ->setBounce(bounce)
    ->setRadius(radius)
    ->enableCollision()
    ->makeFree();
    
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
            float radius = size * 10.f;
            
            if (cur.a > 5.f && radius >= 2.f && ofGetWindowRect().inside(pos)) {
                
                float mass = size * 10.f + min_mass;
                float bounce = size * 0.1f + min_bounce;
                
                auto a = new Particle;
                a->color = ofColor(cur);
                a->setMass(mass)
                ->setBounce(bounce)
                ->setRadius(radius)
                ->enableCollision()
                ->makeFree()->moveTo(pos);
                
                physics.addParticle(a);
                
                a->release();
            }
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
