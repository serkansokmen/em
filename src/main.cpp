#include "ofMain.h"
#include "ofApp.h"


int main(){
    
    int windowWidth = 900;
    int windowHeight = 720;
    
#ifdef TARGET_OPENGLES
    ofGLESWindowSettings settings;
    settings.width = windowWidth;
    settings.height = windowHeight;
    settings.setGLESVersion(2);
    ofCreateWindow(settings);
#else
    ofGLWindowSettings settings;
    settings.width = windowWidth;
    settings.height = windowHeight;
    settings.setGLVersion(2, 1);
    settings.windowMode = OF_FULLSCREEN;
    ofCreateWindow(settings);
#endif
    ofRunApp(new ofApp());
}
