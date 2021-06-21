#include "ofApp.h"
using namespace cv;
#include "opencv2/opencv.hpp"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);

    panel.setup();
    panel.add(distUpperThreshSlider.setup("Dist Upper Thresh", 600, 0, 2000));
    panel.add(distLowerThreshSlider.setup("Dist Lower Thresh", 10, 0, 500));
    panel.add(blurAmntSlider.setup("Blur Amount", 1, 0, 20));
    panel.add(opacSlider.setup("Tri Opacity", 180, 0, 255));
    panel.add(wireOpacSlider.setup("Wire Opacity", 100, 0, 255));
    panel.add(amntPointsSlider.setup("Amount of Points", 80, 3, 400));
    panel.add(blobsRadiusSlider.setup("Points Radius Size", 10, 1, 300));
    
    colMode = 1;
    drawFrame = true;
    drawPoints = false;
    addCorners = false;
    drawImage = true;
    drawGUI = true;
    wireCol = true;
    
    // IMPORT YOUR OWN IMAGE HERE!!
    sourceImg.load("Godfather1.jpg");
    
    topLine = ofPoint(10, 180);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    image.setFromPixels( sourceImg );
        
    //Convert to grayscale image
    grayImage = image;

    //Smoothing image
    blurred = grayImage;
    blurAmnt = blurAmntSlider;
    // Give only odd numbers;
    blurAmnt = (blurAmnt * 2) + 1;
    blurred.blurGaussian( blurAmnt );

//        //Thresholding for obtaining binary image
//        mask = blurred;
//        mask.threshold( ofMap(mouseX, 0, ofGetWidth(), 0, 200) );

//        //Getting an inverted mask of image to use in contour finder
//        inverted = mask;
//        inverted.invert();
        
    float noiseScale = 0.003;
    float noise = ofNoise(ofGetFrameNum() * noiseScale);
        
    int amountPoints = amntPointsSlider;

    //#### FIND INTERESTING POINTS #####
    Mat imageCV;
    imageCV = cvarrToMat(blurred.getCvImage());
    goodFeaturesToTrack(imageCV, corners, amountPoints, 0.01, 4); //param 3 is how many points you want to get
        
    triangulation.reset();
    if (addCorners) {
    // Add Corners;
        triangulation.addPoint(ofPoint(0, 0));
        triangulation.addPoint(ofPoint(0, image.height));
        triangulation.addPoint(ofPoint(image.width, 0));
        triangulation.addPoint(ofPoint(image.width, image.height));
    }
    for (int i = 0; i < corners.size(); i++) {
        triangulation.addPoint(ofPoint(corners[i].x, corners[i].y));
    }
    triangulation.triangulate();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);    //Set the background color

    ofSetColor( 255, 255, 255 );    //Set color for images drawing
    
    opac = opacSlider;
    
    int w = image.width;
    int h = image.height;
    
        ofSetColor(0);
        ofNoFill();
        ofDrawRectangle(0,0,w,h);
        
    if (drawImage) {
        // Draw Original Image Underneath;
        ofSetColor(255);
        image.draw(0, 0, w, h);
    }

    if (drawPoints) {
        //draw the interesting points in RED
        ofFill();
        int pointSize;
        for (int i=0; i<corners.size(); i++) {
            if (colMode == 4) {
                ofColor pointCol(image.getPixels().getColor(corners[i].x, corners[i].y));
                ofSetColor(pointCol);
                pointSize = blobsRadiusSlider;
            } else {
                ofSetColor(255,0,0);
                pointSize = blobsRadiusSlider;
            }
            ofDrawEllipse(corners[i].x, corners[i].y, pointSize, pointSize);
        }
    }
    
    // Draw the Triangles;
    ofFill();
    for (int g=0; g<triangulation.getNumTriangles(); g++){ // loop over the triangles
        vector <glm::vec2> pts = getTriangle(g);             // extract the vector with 3 points
            
        if (colMode == 1) {
            // Get Center Point;
            float sumX = 0;
            float sumY = 0;
            for (int i = 0; i < 3; i++) {
                sumX += pts[i].x;
                sumY += pts[i].y;
            }
            float avgX = sumX / 3;
            float avgY = sumY / 3;
                
            ofFill();
            ofColor centerCol(image.getPixels().getColor(avgX, avgY));
            ofSetColor(centerCol, opac);
        }

        if (colMode == 2) {
            ofFill();
            // Get Color at just pts[0] loc for more abstract;
            ofColor pts0(image.getPixels().getColor(pts[0].x, pts[0].y));
            ofSetColor(pts0, opac);
        }
            
        if (colMode == 3) {
            ofNoFill();
            ofSetLineWidth(3);
            ofColor pts1(image.getPixels().getColor(pts[1].x, pts[1].y));
            ofSetColor(pts1, opac);
            drawFrame = false;
        }
            
        // Lower Threshold;
        int shortestDist = 300;
        for (int i = 0; i < 3; i++) {
            int j = (i + 1) % 3;
            int newDist = ofDist(pts[i].x, pts[i].y, pts[j].x, pts[j].y);
            if (newDist < shortestDist) {
                shortestDist = newDist;
            }
        }
        
        // Upper Threshold;
        int longestDist = 0;
        for (int i = 0; i < 3; i++) {
            int j = (i + 1) % 3;
            int newDist = ofDist(pts[i].x, pts[i].y, pts[j].x, pts[j].y);
            if (newDist > longestDist) {
                longestDist = newDist;
            }
        }
           
        // Update Thresholds with GUI;
        distLowerThresh = distLowerThreshSlider;
        distUpperThresh = distUpperThreshSlider;
    
            if (colMode != 4) {
                if (shortestDist > distLowerThresh && longestDist < distUpperThresh) {
                    ofDrawTriangle(pts[0], pts[1], pts[2]);           // use this point to draw a triangle
                }
            }
        }
    
    // Draw Wire Frame;
    wireOpac = wireOpacSlider;
    if (drawFrame) {
        ofSetLineWidth(1);
        if (wireCol) {
            ofSetColor(0, wireOpac);
        } else {
            ofSetColor(255, wireOpac);
        }
        triangulation.triangleMesh.drawWireframe();
    }
    
    // Draw GUI and Instructions;
    if (drawGUI) {
        panel.draw();
        if (wireCol) {
            ofSetColor(255);
        } else {
            ofSetColor(0);
        }
    
        int spacing = 15;
        ofDrawBitmapString("Welcome to Delaunay Image Editor!", topLine.x, topLine.y);
        ofDrawBitmapString("4 Different Presets, selected by 1,2,3,4", topLine.x, topLine.y + spacing);
        ofDrawBitmapString("Toggle the frame with 'f'", topLine.x, topLine.y + (2 * spacing));
        ofDrawBitmapString("Toggle frame color (and instructions) with 'w'", topLine.x, topLine.y + (3*spacing));
        ofDrawBitmapString("Draw Important points with 'p'", topLine.x, topLine.y + (4*spacing));
        ofDrawBitmapString("Add/Remove corner points with 'c'", topLine.x, topLine.y + (5*spacing));
        ofDrawBitmapString("Remove these instructions and GUI with spacebar", topLine.x, topLine.y + (6*spacing));
        ofDrawBitmapString("CLICK ANYWHERE TO MOVE INSTRUCTIONS", topLine.x, topLine.y + (7*spacing));
        ofDrawBitmapString("and HAVE FUN! :D", topLine.x, topLine.y + (8*spacing));
    }
}
//--------------------------------------------------------------
vector <glm::vec2> ofApp::getTriangle(int i){
    int pA = triangulation.triangleMesh.getIndex(i*3);
    int pB = triangulation.triangleMesh.getIndex(i*3+1);
    int pC = triangulation.triangleMesh.getIndex(i*3+2);

    glm::vec2 pointA = triangulation.triangleMesh.getVertex(pA);
    glm::vec2 pointB = triangulation.triangleMesh.getVertex(pB);
    glm::vec2 pointC = triangulation.triangleMesh.getVertex(pC);

    vector <glm::vec2> points;
    points.push_back(pointA);
    points.push_back(pointB);
    points.push_back(pointC);
    return points;
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == '1') colMode = 1, drawFrame = true;
    if (key == '2') colMode = 2, drawFrame = false, drawPoints = false;
    if (key == '3') colMode = 3, drawPoints = true;
    if (key == '4') colMode = 4, drawPoints = true;
    if (key == 'f') drawFrame = !drawFrame;
    if (key == 'w') wireCol = !wireCol;
    if (key == 'p') drawPoints = !drawPoints;
//    if (key == 'i') drawImage = !drawImage;
    if (key == 'c') addCorners = !addCorners;
    if (key == 'r') ofResetElapsedTimeCounter();
    if (key == ' ') drawGUI = !drawGUI;
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    topLine = ofPoint(x, y);
}
