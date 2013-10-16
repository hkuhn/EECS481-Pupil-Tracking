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
    
    
    //
    // BUILD FACE TRACKER
    //
    cout << "Building face tracker..." << endl;
    
    
    //create the cascade classifier object used for the face detection
    GazeTracker gt;
    gt.train();
    
    
    // create window
    namedWindow("Camera Feed",CV_WINDOW_AUTOSIZE);
    moveWindow("Camera Feed", 400, 100);
    namedWindow("Debugging Window", CV_WINDOW_AUTOSIZE);
    
    // TEMPORARY
    // We need to create threads to allow for concurrent processes
    while (1)
    {
        Mat frame;
        Mat debuggingFrame;
        Mat grayscaleFrame;
        Mat blurredFrame;
        
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from camera" << endl;
            break;
        }
        
        //convert captured image to gray scale and equalize
		cvtColor(frame, grayscaleFrame, CV_BGR2GRAY);
		equalizeHist(grayscaleFrame, grayscaleFrame);
        
        // make debugging frame
        debuggingFrame = frame.clone();

        //create a vector array to store the face found
		vector<Rect> faces;
        vector<vector<Rect> > eyes;
        vector<vector <Point> > pupils;
        

        //find faces and store them in the vector array        
        faces = gt.getFaces(grayscaleFrame);
        
        //locate eye regions from faces vector
        eyes = gt.getEyes(faces);
        
        
        //smooth image w/ Gaussian Blur based on faces
        //detect pupil locations
		//draw a rectangle for all found faces in the vector array on the original image
        //draw a rectangle for all found eyes in the vector
		for(int i = 0; i < faces.size(); i++)
		{
            
            //perform gaussian blur before processing gradients
            Size gaussian_size(5,5);
            double sigma = 0.005*faces[i].width;
            GaussianBlur(grayscaleFrame, blurredFrame, gaussian_size, sigma, sigma, BORDER_DEFAULT);
            
            //perform gradient method for pupil tracking
            vector<Point> current_pupils;
            current_pupils = gt.getPupils(faces[i], eyes[i], blurredFrame);
            pupils.push_back(current_pupils);
            
			Point pt1(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
			Point pt2(faces[i].x, faces[i].y);
            
            Point pt3(eyes[i][0].x + eyes[i][0].width, eyes[i][0].y + eyes[i][0].height);
            Point pt4(eyes[i][0].x, eyes[i][0].y);
            
            Point pt5(eyes[i][1].x + eyes[i][1].width, eyes[i][1].y + eyes[i][1].height);
            Point pt6(eyes[i][1].x, eyes[i][1].y);
            
			rectangle(debuggingFrame, pt1, pt2, cvScalar(0, 255, 0, 0), 1, 8, 0);
            rectangle(debuggingFrame, pt3, pt4, cvScalar(255, 0, 0, 0), 1, 8, 0);
            rectangle(debuggingFrame, pt5, pt6, cvScalar(255, 0, 0, 0), 1, 8, 0);
            
            if (current_pupils[0] != pt4) {
                circle(debuggingFrame, current_pupils[0], 3, cvScalar(0,0,255,0));
            }
            if (current_pupils[1] != pt6) {
                circle(debuggingFrame, current_pupils[1], 3, cvScalar(0,0,255,0));
            }
            
		}
        
        imshow("Camera Feed", frame); //show the frame in "MyVideo" window
        imshow("Debugging Window", debuggingFrame);
 
        
        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }
    }
    
    return;
}






