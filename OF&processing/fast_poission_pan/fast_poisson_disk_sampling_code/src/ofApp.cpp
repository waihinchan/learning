#include "ofApp.h"
#include "Sampler.hpp"
Sampler fast_poission_disk_sampler;

//ofVec2f orginalarray [3] = {ofVec2f(-1, -1),ofVec2f(-2, -1),ofVec2f(-3, -1)};

int scl;

//--------------------------------------------------------------
void ofApp::setup(){
//--------------------------------------------------------------测试智能指针
//    active = std::unique_ptr<ofVec2f[]>(new ofVec2f[3]{ofVec2f(-1, -1),ofVec2f(-2, -1),ofVec2f(-3, -1)});
//    active = new ofVec2f[3]{ofVec2f(-1, -1),ofVec2f(-2, -1),ofVec2f(-3, -1)};
//    active = orginalarray;
//--------------------------------------------------------------

    ofBackground(0);
    fast_poission_disk_sampler.init();
    fast_poission_disk_sampler.sampling();
    myImage.load("bbb.jpg");
    myImage.allocate(myImage.getWidth(), myImage.getHeight(), OF_IMAGE_COLOR);
    ofLoadImage(pxdata, "bbb.jpg");
    
    for(int i = 0;i<fast_poission_disk_sampler.samples.size();i++){
        std::cout<<fast_poission_disk_sampler.samples[i]<<endl;
    }
//    std::cout<<fast_poission_disk_sampler.samples.size()<<endl;
//    std::cout<<pxdata.size()<<endl;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
//--------------------------------------------------------------测试智能指针
//    for(int i = 0 ; i<3;i++){
//        std::cout<<active[i].getNormalized() <<endl;
//    }
//--------------------------------------------------------------
//    fast_poission_disk_sampler.sampling();
//    fast_poission_disk_sampler.visualization();
//--------------------------------------------------------------
    

    
    
    
    for(int i = 0;i<fast_poission_disk_sampler.samples.size();i++){
        
        float xscl = myImage.getHeight()/ofGetWidth();
        float yscl = myImage.getHeight()/ofGetHeight();
        float orginalxpos =floor(fast_poission_disk_sampler.samples[i].x);
        float orginalypos =floor(fast_poission_disk_sampler.samples[i].y);

        int xpos = floor(orginalxpos*xscl);
        int ypos = floor(orginalypos*yscl);
        int index = ypos*myImage.getWidth()+xpos;

        int r = pxdata.getData()[index*3+0];
        int g = pxdata.getData()[index*3+1];
        int b = pxdata.getData()[index*3+2];

        ofSetColor(r, g, b);
        ofDrawCircle(orginalxpos, orginalypos, 3);

    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
