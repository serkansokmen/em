#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCameraSaveLoad.h"
#include "ofxVideoRecorder.h"
#include "Constants.h"


namespace em {
    class SceneCamera {
        void setupScreenFbo(){
            if (!screenFbo.isAllocated()) {
                ofFbo::Settings settings;
                settings.numSamples = 4;
                settings.width = FBO_WIDTH;
                settings.height = FBO_HEIGHT;
                settings.internalformat = GL_RGB32F_ARB;
                settings.useDepth = true;
                settings.useStencil = true;
                screenFbo.allocate(settings);
            }
            screenFbo.begin();
            ofClear(0,0,0,0);
            screenFbo.end();
        }
        void setCamFov(float& v) {
            previewCam.setFov(v);
        }
        void setCamNearClip(float& v) {
            previewCam.setNearClip(v);
        }
        void setCamFarClip(float& v) {
            previewCam.setFarClip(v);
        }
        void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
            cout << "The recoded video file is now complete." << endl;
        }
        
        ofxVideoRecorder    vidRecorder;
        ofFbo               screenFbo;
        ofFbo               recordFbo;
        ofPixels            recordPixels;
        ofEasyCam           previewCam;
        string              fileName;
        string              fileExt;
        bool                bRecording;
        
    public:
        
        ~SceneCamera(){
            ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &SceneCamera::recordingComplete);
            camFov.removeListener(this, &SceneCamera::setCamFov);
            camNearClip.removeListener(this, &SceneCamera::setCamNearClip);
            camFarClip.removeListener(this, &SceneCamera::setCamFarClip);
            vidRecorder.close();
            ofxSaveCamera(previewCam, "preview_cam_settings");
        }
        
        void setup(const ofPoint& lookAt){
            
            params.setName("Scene Camera View");
            params.add(orbitCamera.set("Orbit Camera", false));
            params.add(camFov.set("Field of View", 60, 35.f, 180.f));
            params.add(camNearClip.set("Near Clip", 0.1f, 0.1f, 20.f));
            params.add(camFarClip.set("Far Clip", 5000.f, 20.f, 10000.f));
            
            camFov.addListener(this, &SceneCamera::setCamFov);
            camNearClip.addListener(this, &SceneCamera::setCamNearClip);
            camFarClip.addListener(this, &SceneCamera::setCamFarClip);
            
            // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
            fileName = "recording";
            fileExt = ".mov";
            // override the default codecs if you like
            // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
            
            ofxLoadCamera(previewCam, "preview_cam_settings");
            previewCam.setFov(camFov);
            previewCam.lookAt(lookAt);
            vidRecorder.setVideoCodec("mpeg4");
            vidRecorder.setVideoBitrate("50000k");
            vidRecorder.setFfmpegLocation("/usr/local/bin/ffmpeg");
            //    vidRecorder.setAudioCodec("mp3");
            //    vidRecorder.setAudioBitrate("192k");
            bRecording = false;
            
            setupScreenFbo();
            
            ofAddListener(vidRecorder.outputFileCompleteEvent, this, &SceneCamera::recordingComplete);
            bRecording = false;
        }
        void update(){
            float time = ofGetElapsedTimef();
            if (orbitCamera) {
                float lng = time*10;
                float lat = sin(time/100);
                float radius = previewCam.getGlobalPosition().distance(previewCam.getTarget().getPosition());
                previewCam.orbit(lng, lat, radius);
            }
            if (bRecording) {
                ofPixels pixels;
                screenFbo.readToPixels(pixels);
                bool success = vidRecorder.addFrame(pixels);
                if (!success) {
                    ofLogWarning("This frame was not added!");
                }
            }
            // Check if the video recorder encountered any error while writing video frame or audio smaples.
            if (vidRecorder.hasVideoError()) {
                ofLogWarning("The video recorder failed to write some frames!");
            }
            if (vidRecorder.hasAudioError()) {
                ofLogWarning("The video recorder failed to write some audio samples!");
            }
        }
        void beginScene(){
            // Render to Fbo
            screenFbo.begin();
            ofClear(0,0,0,0);
        }
        void beginCamera(){
            previewCam.begin();
        }
        void endCamera(){
            previewCam.end();
        }
        void endScene(){
            screenFbo.end();
        }
        void draw(float x=0, float y=0, float width=FBO_WIDTH, float height=FBO_HEIGHT){
            screenFbo.draw(x, y, width, height);
            if (bRecording) {
                ofSetColor(255, 0, 0);
                ofDrawCircle(width - 20, 20, 5);
            }
        }
        void draw(ofPoint position, float width, float height){
            draw(position.x, position.y, width, height);
        }
        void draw(ofRectangle rect){
            draw(rect.x, rect.y, rect.width, rect.height);
        }
        
        void addAudioSamplesToRecording(float *input, int bufferSize, int nChannels){
            if (bRecording) {
                vidRecorder.addAudioSamples(input, bufferSize, nChannels);
            }
        }
        const bool& isRecording(){
            return bRecording;
        }
        
        void toggleRecording(){
            bRecording = !bRecording;
            if(bRecording && !vidRecorder.isInitialized()) {
                vidRecorder.setup(fileName+ofGetTimestampString()+fileExt,
                                  FBO_WIDTH, FBO_HEIGHT,
                                  60, 44100, 2, false, false);
                vidRecorder.start();
            }
            else if(!bRecording && vidRecorder.isInitialized()) {
                vidRecorder.setPaused(true);
            }
            else if(bRecording && vidRecorder.isInitialized()) {
                vidRecorder.setPaused(false);
            }
        }
        void endRecording(){
            bRecording = false;
            vidRecorder.close();
        }
        
        ofParameterGroup     params;
        ofParameter<float>   camFov;
        ofParameter<float>   camNearClip;
        ofParameter<float>   camFarClip;
        ofParameter<bool>    orbitCamera;
    };
}