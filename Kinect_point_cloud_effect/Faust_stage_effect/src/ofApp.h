#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinectV2.h"
#include "ofFbo.h"
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
//		kinect
//      std::vector<std::shared_ptr<ofxKinectV2>> kinects;
//      if have two more device need a share pointer to access the member function
        ofxKinectV2 myKinect;
        ofMesh myRobot;
        bool debugMode;
        ofTexture temp_depth_pixels;
        ofEasyCam cam;
//            kinect
    
//    opencv
        bool learnBG = true;
        uint8_t threshold = 20;
        ofImage RGBimage;
        ofxCvGrayscaleImage graybg;
        
        ofxCvColorImage orginal_image;

        ofxCvGrayscaleImage grayimage;

        ofxCvGrayscaleImage graydiff;

        ofxCvContourFinder finder;

        //边缘检测
//    opencv
        ofImage myimage;
        ofPolyline MyOutline;
        void pointinside(ofPolyline mypolyline,ofTexture temppixels);
    
};
