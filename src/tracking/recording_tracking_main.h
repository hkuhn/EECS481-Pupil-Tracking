//  EECS481: DrawFree - Pupil Tracking
//  recording_tracking_main.h
//
//  
//
//
//
//

#ifndef ____recording_tracking_main__
#define ____recording_tracking_main__

#include <iostream>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

// FUNCTION DECLARATIONS
void recording_tracking_main(const char* path);
vector<vector<Rect> > locateEyes(vector<Rect> faces);
vector< Point> locatePupils(Rect face, vector<Rect> eyes, Mat blurredFrame);


#endif /* defined(____recording_tracking_main__) */
