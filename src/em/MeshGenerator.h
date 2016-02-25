#pragma once

#include "ofMain.h"
#include "MSAPhysics3D.h"
#include "ofxAnimatableOfPoint.h"
#include "Constants.h"


namespace em {
    class MeshGenerator {
        
    private:
        
        void updateShading(){
            polyMat.setAmbientColor(polygonAmbient);
            polyMat.setDiffuseColor(polygonDiffuse);
            polyMat.setSpecularColor(polygonSpecular);
            polyMat.setShininess(polygonShininess);
            
            springMat.setAmbientColor(springAmbient);
            springMat.setDiffuseColor(springDiffuse);
            springMat.setSpecularColor(springSpecular);
            springMat.setShininess(springShininess);
        }
        
        void updatePhysics(){
            int numParticles = physics.numberOfParticles();
            int numSprings = physics.numberOfSprings();
            int numAttractions = physics.numberOfAttractions();
            
            float dt = 1.0f / 60.0f;
            fixedParticlePos.update(dt);
            fixedParticle.moveTo(fixedParticlePos.getCurrentPosition());
            particleCount.set(numParticles);
            springCount.set(numSprings);
            attractionCount.set(numAttractions);
            
            if (!physicsPaused) {
                physics.update();
            }
            
            polyMesh.clear();
            polyMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            for(int i=0; i<numParticles; i++){
                auto p = physics.getParticle(i);
                polyMesh.addVertex(p->getPosition());
                polyMesh.addColor(polyMat.getDiffuseColor());
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
        
        void setZDepth(float& v) {
            for (int i=0; i<physics.numberOfParticles(); i++) {
                auto p = physics.getParticle(i);
                ofPoint pos(p->getPosition());
                pos.z = ofRandom(-v, v);
                p->moveTo(pos);
            }
        }
        void setGravityVec(ofPoint& g){
            physics.setGravity(g);
        }
        template <typename T>
        void makeSpringBetweenParticles(msa::physics::ParticleT<T> *a,
                                        msa::physics::ParticleT<T> *b){
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
        
        
        
        // Physics
        msa::physics::World3D       physics;
        msa::physics::Particle3D    fixedParticle;
        ofxAnimatableOfPoint        fixedParticlePos;
        
        // Mesh
        of3dPrimitive        polyPrimitive;
        of3dPrimitive        springPrimitive;
        
        // Shading
        ofVboMesh            polyMesh, springMesh;
        ofShader             polyShader, springShader;
        ofMaterial           polyMat, springMat;
        
    public:
        
        MeshGenerator(){
            fixedParticlePos.setPosition(ofPoint(0,0,0));
            fixedParticlePos.setRepeatType(PLAY_ONCE);
            fixedParticlePos.setCurve(EXPONENTIAL_SIGMOID_PARAM);
            
            physics.setSectorCount(SECTOR_COUNT);
            physics.setTimeStep(60);
            
            physics.setDrag(0.97f);
            physics.disableCollision();
            
            physics.addParticle(&fixedParticle);
            fixedParticle.setMass(1)->setRadius(10.f)->moveTo(ofPoint::zero())->makeFixed();
        }
        
        ~MeshGenerator(){
            zDepth.removeListener(this, &MeshGenerator::setZDepth);
            gravity.removeListener(this, &MeshGenerator::setGravityVec);
            boxSize.removeListener(this, &MeshGenerator::setPhysicsBoxSize);
        }
        void setup(){
            params.setName("Mesh Generator");
            params.add(boxSize.set("Box size", 100.0, 1.0, 2000.0));
            
            params.add(physicsPaused.set("Paused", false));
            params.add(gravity.set("Gravity", ofPoint(0, 0, 0), ofPoint(-1, -1, -1), ofPoint(1, 1, 1)));
            params.add(attraction.set("Attraction", MIN_ATTRACTION, MIN_ATTRACTION, MAX_ATTRACTION));
            params.add(bindToFixedParticle.set("Bind to center", true));
            params.add(radius.set("Particle Radius", PARTICLE_MIN_RADIUS, PARTICLE_MIN_RADIUS, PARTICLE_MAX_RADIUS));
            params.add(mass.set("Particle Mass", MIN_MASS, MIN_MASS, MAX_MASS));
            params.add(bounce.set("Particle Bounce", MIN_BOUNCE, MIN_BOUNCE, MAX_BOUNCE));
            params.add(drag.set("Drag", 0.97, 0.0, 1.0));
            params.add(springStrength.set("Spring Strength", SPRING_MIN_STRENGTH, SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH));
            params.add(springLength.set("Spring Length", SPRING_MIN_LENGTH, SPRING_MIN_LENGTH, SPRING_MAX_LENGTH));
            params.add(zDepth.set("Z Depth", 50, 0, 400));
            params.add(makeParticles.set("Make Particles", true));
            params.add(makeSprings.set("Make Springs", true));
            params.add(particleCount.set("Particle Count", 0));
            params.add(springCount.set("Spring Count", 0));
            params.add(attractionCount.set("Attraction Count", 0));
            
            params.add(polygonAmbient.set("Polygon Ambient", ofFloatColor(1,1,1,.1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
            polygonDiffuse.set("Diffuse", ofFloatColor(0.8,0.8,0.8,1.0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
            polygonSpecular.set("Specular", ofFloatColor(0.8,0.8,0.8,1.0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
            params.add(polygonShininess.set("Polygon Shininess", 10, 0, 255));
            
            params.add(springAmbient.set("Spring Ambient", ofFloatColor(1,1,1,.1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
            springDiffuse.set("Diffuse", ofFloatColor(1.0,1.0,1.0,1.0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
            springSpecular.set("Specular", ofFloatColor(0.8,0.8,0.8,1.0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
            springShininess.set("Spring Shininess", 10, 0, 255);
            
            boxSize.addListener(this, &MeshGenerator::setPhysicsBoxSize);
            zDepth.addListener(this, &MeshGenerator::setZDepth);
            gravity.addListener(this, &MeshGenerator::setGravityVec);
        }
        
        void update(){
            updateShading();
            updatePhysics();
        }
        
        void draw(bool drawPolyMesh=true, bool drawSpringMesh=true, bool drawWireframe=false){
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
//                    float distToCam = p->getPosition().distance(previewCam.getGlobalPosition()) / 10;
//                    ofSetColor(255.0, ofClamp(255 - distToCam, 0, 255));
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
        
        void clear(){
            physics.clear();
            physics.addParticle(&fixedParticle);
        }
        
        void saveMesh(bool savePolyMesh=true, bool saveSpringMesh=true){
            if (savePolyMesh)       polyMesh.save("polyMesh.ply");
            if (saveSpringMesh)     springMesh.save("springMesh.ply");
        }
        
        void randomiseParams(){
            radius.set(ofRandom(radius.getMin(), radius.getMax()));
            mass.set(ofRandom(mass.getMin(), mass.getMax()));
            bounce.set(ofRandom(bounce.getMin(), bounce.getMax()));
            attraction.set(ofRandom(attraction.getMin(), attraction.getMax()));
            springStrength.set(ofRandom(springStrength.getMin(), springStrength.getMax()));
            springLength.set(ofRandom(springLength.getMin(), springLength.getMax()));
        }
        
        //--------------------------------------------------------------
        void makeParticleAtCenter(float r){
            auto a = new msa::physics::Particle3D;
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
        void makeParticleAtPosition(const ofPoint& p){
            auto a = new msa::physics::Particle3D;
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
        void makeCluster(){
            
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

        void setPhysicsBoxSize(double& s){
            physics.setWorldSize(ofVec3f(-s, -s, -s), ofVec3f(s, s, s));
        }
        
        ofPoint getFixedParticlePosition(){
            return fixedParticle.getPosition();
        }
        
        ofParameterGroup params;
        ofParameter<double>  boxSize;
        
        // Physics params
        ofParameter<ofPoint> gravity;
        ofParameter<double>  radius;
        ofParameter<double>  drag;
        ofParameter<double>  mass;
        ofParameter<double>  bounce;
        ofParameter<double>  attraction;
        ofParameter<double>  springStrength;
        ofParameter<double>  springLength;
        ofParameter<float>   zDepth;
        ofParameter<int>     particleCount;
        ofParameter<int>     springCount;
        ofParameter<int>     attractionCount;
        ofParameter<bool>    makeParticles, makeSprings;
        ofParameter<bool>    bindToFixedParticle;
        ofParameter<bool>    physicsPaused;
        
        // Shading
        ofParameter<ofFloatColor>   polygonAmbient, polygonDiffuse, polygonSpecular;
        ofParameter<ofFloatColor>   springAmbient, springDiffuse, springSpecular;
        ofParameter<float>          polygonShininess, springShininess;
    };
}