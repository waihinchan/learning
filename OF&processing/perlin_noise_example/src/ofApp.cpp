#include "ofApp.h"

std::vector<dot> dots;
int scl = 10;
int rows;
int cols;
std::vector<ofVec2f> forcefield;
float xoff = 0;
float yoff = 0;
float zoff = 0;
float r;
//

//--------------------------------------------------------------
void ofApp::setup(){
//    ofSetFrameRate(30);
    ofBackground(20,20,20);
    ofSetBackgroundAuto(false);
    rows = int(ofGetWidth()/scl);
    cols = int(ofGetHeight()/scl);

    for(int i =0;i<rows*cols;i++){
        forcefield.push_back(ofVec2f(0, 0));
    }
    
    for(int i = 0; i < 3000; i++){

        dots.push_back(dot());
        std::cout<<dots[i].pos<<endl;
    }

}

//--------------------------------------------------------------
void ofApp::update(){
        yoff = 0;
        for(int i = 0; i < rows; i++){
            xoff = 0;
            for(int j = 0; j <cols;j++){
                float angle = ofNoise(xoff,yoff,zoff)*TWO_PI*4;
                forcefield[j*rows+i]=ofVec2f(cos(angle)*1.5,sin(angle)*1.5);
//                std:cout<<forcefield[j*rows+i].x<<endl;
                xoff+=0.05;
        }
            yoff+=0.05;
            zoff+=0.05;
        }
//
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetLineWidth(1);
    for(int i = 0; i<dots.size();i++){
        int xindex = floor(dots[i].pos.x/scl);
        int yindex = floor(dots[i].pos.y/scl);
        int index = yindex*rows+xindex;
        dots[i].applyforce(forcefield[index]);
        dots[i].updatepos();
        dots[i].edge();
        dots[i].draw(5);

    }
    
//-------------------------------------------------------------- fortest
//    ofSetColor(255, 255, 255);
//    ofSetLineWidth(1);
//    for(int i = 0; i < rows; i++){
//        for(int j = 0; j <cols;j++){
//            ofPushMatrix();
//            ofTranslate(i*scl, j*scl);
//            ofDrawLine(0, 0, forcefield[j*rows+i].x*10, forcefield[j*rows+i].y*10);
//            ofPopMatrix();
//
//        }
//    }
//--------------------------------------------------------------fortest
//            ofSetColor(255, 255, 255);
//
//            for(int i = 0; i < 10; i++){
//                r = ofNoise(xoff)*20;
//                std::cout<<i<<" "<<r<<endl;
//                ofPushMatrix();
//                ofTranslate(i*50+500, ofGetHeight()/2);
//                ofDrawCircle(0, 0, r);
//                ofPopMatrix();
//                xoff+=0.05;
//
//            }
//--------------------------------------------------------------
}
//--------------------------------------------------------------

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
