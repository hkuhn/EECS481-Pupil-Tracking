//
//  GazeTracker.h
//  
//
//  Created by Hobey Kuhn on 10/14/13.
//
//

#ifndef ____GazeTracker__
#define ____GazeTracker__


/////////////////////////////////////////////
// LIBRARIES
/////////////////////////////////////////////

// C++ Reference
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <stdio.h>
#include <math.h>

// OpenCV
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


/////////////////////////////////////////////
// NAMESPACE
/////////////////////////////////////////////
using namespace std;
using namespace cv;




/////////////////////////////////////////////
// CLASSDEF
/////////////////////////////////////////////
class GazeTracker {
    
    // OBJECTS
    CascadeClassifier face_cascade;
    
public:
    void train();
    void train(string pathToFaces);
    vector<Rect> getFaces(Mat grayscaleFrame);
    vector<vector<Rect> > getEyes(vector<Rect> faces);
    vector<Point> getPupils(Rect face, vector<Rect> eyes, Mat blurredFrame);
    
private:
    // IMPLEMENTATION FUNCTIONS
    vector<vector<Rect> > locateEyes(vector<Rect> faces);
    vector<Point> locatePupils(Rect face, vector<Rect> eyes, Mat blurredFrame);
    
    // HELPER FUNCTIONS (helper.cpp)
    bool rectInImage(Rect rect, Mat image);
    bool inMat(Point p, int rows, int cols);
    Mat matrixMagnitude(const Mat &matX, const Mat &matY);
    double computeDynamicThreshold(const Mat &mat, double stdDevFactor);
    
    // PUPIL FUNCTIONS
    Point unscalePoint(Point p, Rect origSize);
    void scaleToFastSize(const Mat &src, Mat &dst);
    Mat computeMatXGradient(const Mat &mat);
    void testPossibleCentersFormula(int x, int y, unsigned char weight,double gx, double gy, Mat &out);
    Point findEyeCenter(Mat face, Rect eye, string debugWindow);
    bool floodShouldPushPoint(const Point &np, const Mat &mat);
    Mat floodKillEdges(Mat &mat);
    
    
}; //endclass





#endif /* defined(____GazeTracker__) */
