#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    debugMode = true;
// ************************************************************************************ Kinect setup

    ofxKinectV2::Settings mySettings; 
//        remian to modify
//        mySettings.enableRGB = true;
//        mySettings.enableRGBRegistration = true;
//        mySettings.enableIR = false;
//        mySettings.enableDepthRegistration = true;
//        remian to modify

    if(myKinect.getDeviceList().size()>0){
        myKinect.open(myKinect.getDeviceList()[0].serial,mySettings);
    }
    else{
        ofLogWarning()<<"请插入Kinect";
    }
// ************************************************************************************ Kinect setup
// ************************************************************************************ framediff
    int tempw = 512;
    int temph = 424;
    orginal_image.allocate(tempw, temph);
    graybg.allocate(tempw, temph);
    grayimage.allocate(tempw, temph);
    graydiff.allocate(tempw, temph);
    ofLogNotice()<<"width:"<<tempw<<"height:"<<temph;
    
// ************************************************************************************ framediff
}

//--------------------------------------------------------------
void ofApp::update(){
    myKinect.update();
    if(myKinect.isFrameNew()){
        if(debugMode){
            ofLogNotice()<<"myKinectg is Frame new";
        }
        
//       unknow reason we need to swap the RG
        ofPixels temppixel;
        temppixel = myKinect.getRegisteredPixels();
//        temppixel.setNumChannels(3);
        temppixel.swapRgb();
        temppixel.setImageType(OF_IMAGE_COLOR);
        orginal_image.setFromPixels(temppixel);
//       unknow reason  we need to swap the RG
        
        grayimage = orginal_image;
        
        if(learnBG){
            graybg = orginal_image;
            learnBG = false;
            printf("learn");
        }
        graydiff.absDiff(graybg,grayimage);
        //some trick to eliminate the noise
        graydiff.blur();
        graydiff.blur();
        graydiff.threshold(threshold);
        graydiff.erode();
        graydiff.erode();
        graydiff.erode();
        graydiff.erode();
        graydiff.erode();
        graydiff.erode();
        graydiff.dilate();
        graydiff.dilate();
        graydiff.dilate();
        graydiff.dilate();
        finder.findContours(graydiff, 500, 512*424*3/4, 1, true);
        
        //opencv
        
        
        myRobot.clear();
        temp_depth_pixels.loadData(myKinect.getRegisteredPixels());
        
        if(finder.nBlobs>0){
            //push vertex into polyline
            for(int i = 0; i < finder.nBlobs;i++){
                for(int j = 0; j < finder.blobs[i].nPts;j++){
                    MyOutline.addVertex(finder.blobs[i].pts[j].x,finder.blobs[i].pts[j].y);
                }
            }
            MyOutline.simplify();
            MyOutline.getSmoothed(10);
            MyOutline.close();
            ofApp::pointinside(MyOutline,temp_depth_pixels);
            MyOutline.clear();
        }

            // whole scene
//            for(std::size_t x = 0; x< temp_depth_pixels.getWidth();x++){
//                 for(std::size_t y = 0; y < temp_depth_pixels.getHeight();y++){
//                         myRobot.addVertex(myKinect.getWorldCoordinateAt(x, y));
//                         myRobot.addColor(myKinect.getRegisteredPixels().getColor(x, y));
//
//                 }
//             }
            // whole scene
    
    }

        
        
    
}
        

//--------------------------------------------------------------

void ofApp::pointinside(ofPolyline mypolyline,ofTexture temppixels){
    ofRectangle bounds = mypolyline.getBoundingBox();
    int x1 = bounds.getMinX()<0?0:bounds.getMinX();
    int y1 = bounds.getMinY()<0?0:bounds.getMinY();
    int x2 = bounds.getMaxX()>512?temppixels.getWidth():512;
    int y2 = bounds.getMaxY()>424?temppixels.getHeight():424;
    ofPath path;
    for(int i = 0 ; i < mypolyline.getVertices().size(); i ++){
        if(i == 0) path.moveTo(mypolyline.getVertices()[i] );
         else path.lineTo( mypolyline.getVertices()[i] );
    }

    path.close(); // whether can use polyline as fbo
    ofPixels pixels;
    ofFbo fbo;
    pixels.allocate(512,424,OF_IMAGE_GRAYSCALE);
    int samples = fbo.maxSamples();//what is MSAA
    fbo.allocate(pixels.getWidth(),pixels.getHeight(),GL_RGB,samples);
    //turn the outline into white
    fbo.begin();
    ofClear(0,0,0);
    ofSetColor(255,0,0);
    path.draw();
    fbo.end();
    fbo.readToPixels(pixels);
    //turn the outline into white
    if(debugMode){
        myimage.setFromPixels(pixels);
    }
    
    

    

    for(int y = y1; y < y2; y++){
        for(int x = x1; x < x2; x++){
            if(pixels.getColor(x, y).r==255){
//            if(mypolyline.inside(x,y)){//this is very slow
                
                    myRobot.addVertex(myKinect.getWorldCoordinateAt(x, y));
//                    myRobot.addColor(myKinect.getRegisteredPixels().getColor(x, y));//orginal color
                    ofColor mycolor;
                    mycolor.set(10,50, 105);
                    myRobot.addColor(mycolor);

            }
        }
    }

    

}
//--------------------------------------------------------------
void ofApp::draw(){

    if(debugMode){
        if(MyOutline.getVertices().size()>0){
            MyOutline.draw();
            MyOutline.clear();
        }
            graydiff.draw(0, 0);
            orginal_image.draw(0,424);
        
        if(myimage.isAllocated()){
            myimage.draw(512, 0);
        }
    }
    


    
    if(!debugMode){
        cam.begin();
        ofPushMatrix();
        ofScale(100, -100, -100);
        myRobot.setMode(OF_PRIMITIVE_POINTS);
        myRobot.draw();
        ofPopMatrix();
        cam.end();
    }

    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //这里写一个平移相机的功能
    switch (key){
        case ' ':
            learnBG = true;
            break;
        case '+':
            threshold ++;
            if (threshold > 255) threshold = 255;
            break;
        case '-':
            threshold --;
            if (threshold < 0) threshold = 0;
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
