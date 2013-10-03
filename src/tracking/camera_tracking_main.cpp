//  EECS481: DrawFree - Pupil Tracking
//  camera_tracking_main.cpp
//  
//
//  Description:
//      Function house for tracking pupils using a webcam or external camera.
//
//  NOTE:
//      HASN'T WORKED ON MY MACBOOK BUILT-IN ISIGHT CAMERA YET...
//



/////////////////////////////////////////////
// HEADER FILES
/////////////////////////////////////////////
#include "camera_tracking_main.h"



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




void camera_tracking_main() {
    
    // initialize camera
    VideoCapture cap(0); // open the video camera no. 0
    
    // test capture success
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the camera" << endl;
        return;
    }
    
    // retrieve camera dimensions
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    
    // display frame size
    cout << "Frame size : " << dWidth << " x " << dHeight << endl;
    
    // create window
    namedWindow("Camera",CV_WINDOW_AUTOSIZE);
    
    // TEMPORARY
    // We need to create threads to allow for concurrent processes
    while (1)
    {
        Mat frame;
        
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from camera" << endl;
            break;
        }
        
        imshow("Camera", frame); //show the frame in "MyVideo" window
        
        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }
    }
    
    return;
}






