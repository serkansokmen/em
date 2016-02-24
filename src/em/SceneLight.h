#pragma once

#include "ofMain.h"
#include "Constants.h"


namespace em {
    class SceneLight {
    protected:
        shared_ptr<ofLight> light;
        
    public:
        
        SceneLight(){
            light = shared_ptr<ofLight>(new ofLight);
        }
        
        void setup(const int& index){
            
            light->setup();
            
            params.setName("Light " + ofToString(index));
            params.add(enabled.set("Enabled", true));
            params.add(orbit.set("Orbit", true));
            params.add(attConstant.set("Constant Attenuation", 1.0, 0.0, 1.0));
            params.add(attLinear.set("Linear Attenuation", 0.0001, 0.0, 0.01));
            params.add(attQuadratic.set("Quadratic Attenuation", 0.0001, 0.0, 0.001));
            params.add(ambient.set("Ambient", ofFloatColor(1,1,1,.1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
            params.add(orbitSpeed.set("Orbit Speed", 0.1, 0.01, 1.0));
            params.add(orbitRadius.set("Orbit Radius", 1.0, 0.1, 2.0));
            
            diffuse.set("Diffuse", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
            specular.set("Specular", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1));
        }
        
        void update(const float& boxSize){
            if (enabled) {
                light->enable();
                light->setAreaLight(boxSize/2, boxSize/2);
                light->setAttenuation(attConstant, attLinear, attQuadratic);
                light->setAmbientColor(ambient);
                light->setDiffuseColor(diffuse);
                light->setSpecularColor(specular);
                
                if (orbit) {
                    float time = ofGetElapsedTimef();
                    float bs = boxSize / 2;
                    double s = time * 0.8 * orbitSpeed;
                    double c = time * 0.4 * orbitSpeed;
                    float lat = sin(s) * bs;
                    float lng = cos(c) * bs;
                    float rad = boxSize * orbitRadius;
                    light->orbit(lng, lat, rad);
                }
                
            } else {
                light->disable();
            }
        }
        
        void draw(){
            if (enabled) light->draw();
        }
        
        ofParameterGroup            params;
        ofParameter<ofFloatColor>   ambient, diffuse, specular;
        ofParameter<float>          attConstant, attLinear, attQuadratic;
        ofParameter<float>          orbitRadius;
        ofParameter<float>          orbitSpeed;
        ofParameter<bool>           enabled, orbit;
    };
}