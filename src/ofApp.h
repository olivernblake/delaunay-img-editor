#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
//#include "ofxCv.h"
#include "ofxDelaunay.h"
#include "ofxGui.h"

using namespace cv;

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    vector <glm::vec2> getTriangle(int i);
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    
    ofxPanel panel;
    ofxIntSlider distLowerThreshSlider;
    ofxIntSlider distUpperThreshSlider;
    ofxIntSlider blurAmntSlider;
    ofxIntSlider opacSlider;
    ofxIntSlider wireOpacSlider;
    ofxIntSlider amntPointsSlider;
    ofxFloatSlider imgScaleSlider;
    ofxFloatSlider blobsRadiusSlider;
    
    ofImage sourceImg;
    ofxCvColorImage image;

    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage inverted;
    ofxCvGrayscaleImage blurred;
    ofxCvGrayscaleImage mask;

    vector<Point2f> corners;
    
    ofxDelaunay triangulation;
    
    int colMode;
    bool drawFrame;
    bool drawPoints;
    bool drawImage;
    bool drawGUI;
    bool addCorners;
    
    int distLowerThresh;
    int distUpperThresh;
    int blurAmnt;
    int opac;
    int wireOpac;
    float imageScale;
    bool wireCol;
    float blobsRad;
    
    ofPoint topLine;
};

