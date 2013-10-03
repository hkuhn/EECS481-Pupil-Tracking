//  EECS481: DrawFree - Pupil Tracking
//  recording_tracking_main.cpp
//  
//
//
//  Description:
//      Function house for tracking pupils using a pre-recorded video.
//
//


/////////////////////////////////////////////
// HEADER FILES
/////////////////////////////////////////////
#include "recording_tracking_main.h"



/////////////////////////////////////////////
// LIBRARIES
/////////////////////////////////////////////

// C++ Reference
#include <iostream>
#include <string>

// OpenCV
#include "opencv2/highgui/highgui.hpp"



/////////////////////////////////////////////
// NAMESPACE
/////////////////////////////////////////////
using namespace std;
using namespace cv;



void recording_tracking_main(const char* path) {
    
    // retrieve feed
    cout << "Attempting to locate video..." << endl;
    VideoCapture feed(path);
    
    // test validity of feed
    if ( !feed.isOpened() ) {
        cout << "Cannot open the video file" << endl;
        return;
    }
    
    // retrieve frames per second
    double fps = feed.get(CV_CAP_PROP_FPS);
    
    // display information
    cout << "Video found. Press esc to quit program." << endl;
    
    namedWindow("Pupil Tracking",CV_WINDOW_AUTOSIZE);
    
    // TEMPORARY
    // We need to create threads to allow for concurrent processes
    while(1)
    {
        Mat frame;
        
        bool bSuccess = feed.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read the frame from video file" << endl;
            break;
        }
        
        imshow("Video", frame); //show the frame in "MyVideo" window
        
        if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }
    }
    
    
    return;
}







