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

// FUNCTION DECLARATIONS
void recording_tracking_main(const char* path);
void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels);


#endif /* defined(____recording_tracking_main__) */
