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
        
		//create a vector array to store the face found
		vector<Rect> faces;
        vector<vector<Rect> > pupils;
        
		//find faces and store them in the vector array
		face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30));
        
        //locate eye regions from faces vector
        pupils = locateEyes(faces);
        
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
            
            
			Point pt1(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
			Point pt2(faces[i].x, faces[i].y);
            
            Point pt3(pupils[i][0].x + pupils[i][0].width, pupils[i][0].y + pupils[i][0].height);
            Point pt4(pupils[i][0].x, pupils[i][0].y);
            
            Point pt5(pupils[i][1].x + pupils[i][1].width, pupils[i][1].y + pupils[i][1].height);
            Point pt6(pupils[i][1].x, pupils[i][1].y);
            
			rectangle(frame, pt1, pt2, cvScalar(0, 255, 0, 0), 1, 8, 0);
            rectangle(frame, pt3, pt4, cvScalar(255, 0, 0, 0), 1, 8, 0);
            rectangle(frame, pt5, pt6, cvScalar(255, 0, 0, 0), 1, 8, 0);
            
		}
        
        imshow("Video", frame); //show the frame in "MyVideo" window
        
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
    vector<vector<Rect> > pupils;
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
        
        pupils.push_back(insert);
        
    }
    
    
    return pupils;
    
}





