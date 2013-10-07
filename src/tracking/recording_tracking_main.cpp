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
#include <fstream>
#include <sstream>
#include <string>

// OpenCV
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



/////////////////////////////////////////////
// NAMESPACE
/////////////////////////////////////////////
using namespace std;
using namespace cv;



void recording_tracking_main(const char* path) {
    
    //
    // LOCATE VIDEO FEED
    //
    // retrieve feed
    cout << "Attempting to locate video..." << endl;
    VideoCapture feed(path);
    
    // test validity of feed
    if ( !feed.isOpened() ) {
        cout << "Cannot open the video file" << endl;
        return;
    }       
    cout << "Video found" << endl;
    // retrieve frames per second
    double fps = feed.get(CV_CAP_PROP_FPS);
    
    
    //
    // BUILD FACE TRACKER
    //
    cout << "Building face tracker..." << endl;
	//create the cascade classifier object used for the face detection
	CascadeClassifier face_cascade;
	//use the haarcascade_frontalface_alt.xml library
	face_cascade.load("../data/haarcascade_frontalface_alt.xml");
    
    
    
    //
    // RUN FACE TRACKING ON VIDEO FEED
    //
    // display information
    cout << "Running video feed..." << endl;
    cout << "Press esc to quit program." << endl;
    
    namedWindow("Pupil Tracking",CV_WINDOW_AUTOSIZE);
    
    // TEMPORARY
    // We need to create threads to allow for concurrent processes
    while(1)
    {
        Mat frame;
        Mat grayscaleFrame;
        
        bool bSuccess = feed.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read the frame from video file" << endl;
            break;
        }
        
        //convert captured image to gray scale and equalize
		cvtColor(frame, grayscaleFrame, CV_BGR2GRAY);
		equalizeHist(grayscaleFrame, grayscaleFrame);
        
		//create a vector array to store the face found
		std::vector<Rect> faces;
        
		//find faces and store them in the vector array
		face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30));
        
		//draw a rectangle for all found faces in the vector array on the original image
		for(int i = 0; i < faces.size(); i++)
		{
			Point pt1(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
			Point pt2(faces[i].x, faces[i].y);
            
			rectangle(frame, pt1, pt2, cvScalar(0, 255, 0, 0), 1, 8, 0);
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



