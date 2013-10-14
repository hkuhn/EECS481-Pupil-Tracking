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
#include "findEyeCenter.h"
#include "constants.h"



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
    
    namedWindow("Video Feed",CV_WINDOW_AUTOSIZE);
    moveWindow("Video Feed", 400, 100);
    namedWindow("Debugging Window", CV_WINDOW_AUTOSIZE);
    /*
    namedWindow("Right Eye",CV_WINDOW_NORMAL);
    moveWindow("Right Eye", 10, 600);
    namedWindow("Left Eye",CV_WINDOW_NORMAL);
    moveWindow("Left Eye", 100, 600);
     */
    
    int fps_count;
    
    // TEMPORARY
    // We need to create threads to allow for concurrent processes
    while(1)
    {
        Mat frame;
        Mat debuggingFrame;
        Mat grayscaleFrame;
        Mat blurredFrame;
        
        bool bSuccess = feed.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read the frame from video file" << endl;
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
		face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30));
        
        //locate eye regions from faces vector
        eyes = locateEyes(faces);
    
        
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
            current_pupils = locatePupils(faces[i], eyes[i], blurredFrame);
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
        
        imshow("Video Feed", frame); //show the frame in "MyVideo" window
        imshow("Debugging Window", debuggingFrame);
        
        if(waitKey(1) == 27) //wait for 'esc' key press for 1 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }
    }
    
    
    return;
}



// LOCATE EYES
vector<vector<Rect> > locateEyes(vector<Rect> faces) {
    // this function locates the eye regions from face bounding boxes
    // suggested by Timm in his implementation:
    //  let (x,y) be upper left corner of face detection
    //  let W, H be width and height of face detection
    //      mean of right eye center: (x+0.3W, y+0.4H)
    //      mean of left eye center: (x+0.7W, y+0.4H)
    //  
    vector<vector<Rect> > eyes;
    double eye_box_const = 0.12;
    
    // for all faces in vector, find eye boxes
    for (int i =0; i < faces.size(); i++) {
        
        Rect cur_face = faces.at(i);
        int x = faces[i].x;
        int y = faces[i].y;
        int W = faces[i].width;
        int H = faces[i].height;
        int eye_width = floor(eye_box_const*W);
        int eye_height = floor(eye_box_const*H);
        
        int right_mean[] = {floor(x+0.3*W), floor(y+0.4*H)};
        int left_mean[] = {floor(x+0.7*W), floor(y+0.4*H)};
        
        Point right_ul_corner(right_mean[0] - eye_width, right_mean[1] - eye_height);
        Point right_br_corner(right_mean[0] + eye_width, right_mean[1] + eye_height);
        Point left_ul_corner(left_mean[0] - eye_width, left_mean[1] - eye_height);
        Point left_br_corner(left_mean[0] + eye_width, left_mean[1] + eye_height);
        
        Rect right_eye_box(right_ul_corner, right_br_corner);
        Rect left_eye_box(left_ul_corner, left_br_corner);
        
        vector<Rect> insert;
        insert.push_back(right_eye_box);
        insert.push_back(left_eye_box);
        
        eyes.push_back(insert);
        
    }
    
    
    return eyes;
    
}


// LOCATE PUPILS
vector<Point> locatePupils(Rect face, vector<Rect> eyes, Mat blurredFrame) {
    // Find Eye Centers
    Point rightPupil = findEyeCenter(blurredFrame,eyes[0],"Right Eye");
    Point leftPupil = findEyeCenter(blurredFrame,eyes[1],"Left Eye");
    
    // get corner regions
    int eye_region_width = face.width * (kEyePercentWidth/100.0);
    int eye_region_height = face.width * (kEyePercentHeight/100.0);
    int eye_region_top = face.height * (kEyePercentTop/100.0);
    
    Rect leftRightCornerRegion(eyes[1]);
    leftRightCornerRegion.width -= leftPupil.x;
    leftRightCornerRegion.x += leftPupil.x;
    leftRightCornerRegion.height /= 2;
    leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
    
    Rect leftLeftCornerRegion(eyes[1]);
    leftLeftCornerRegion.width = leftPupil.x;
    leftLeftCornerRegion.height /= 2;
    leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
    
    Rect rightLeftCornerRegion(eyes[0]);
    rightLeftCornerRegion.width = rightPupil.x;
    rightLeftCornerRegion.height /= 2;
    rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
    
    Rect rightRightCornerRegion(eyes[0]);
    rightRightCornerRegion.width -= rightPupil.x;
    rightRightCornerRegion.x += rightPupil.x;
    rightRightCornerRegion.height /= 2;
    rightRightCornerRegion.y += rightRightCornerRegion.height / 2;

    // change eye centers to face coordinates
    rightPupil.x += eyes[0].x;
    rightPupil.y += eyes[0].y;
    leftPupil.x += eyes[1].x;
    leftPupil.y += eyes[1].y;

    
    

    vector<Point> ret;
    ret.push_back(rightPupil);
    ret.push_back(leftPupil);
    return ret;
    
}





