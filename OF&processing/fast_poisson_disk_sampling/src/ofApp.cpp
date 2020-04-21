#include "ofApp.h"
#include "Sampler.hpp"
Sampler fast_poission_disk_sampler;
std::unique_ptr<ofVec2f[]> active;
//ofVec2f *active;
//ofVec2f orginalarray [3] = {ofVec2f(-1, -1),ofVec2f(-2, -1),ofVec2f(-3, -1)};
//测试智能指针


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    fast_poission_disk_sampler.init();

    
    
//--------------------------------------------------------------测试智能指针
//    active = std::unique_ptr<ofVec2f[]>(new ofVec2f[3]{ofVec2f(-1, -1),ofVec2f(-2, -1),ofVec2f(-3, -1)});
//    active = new ofVec2f[3]{ofVec2f(-1, -1),ofVec2f(-2, -1),ofVec2f(-3, -1)};
//    active = orginalarray;
//--------------------------------------------------------------

    
    
    
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
    fast_poission_disk_sampler.sampling();
    fast_poission_disk_sampler.visualization();
    
    
    
    
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
