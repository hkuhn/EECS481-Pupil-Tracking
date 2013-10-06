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
#include "helper.hpp"
#include "eigenfaces.hpp"



/////////////////////////////////////////////
// LIBRARIES
/////////////////////////////////////////////

// C++ Reference
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// OpenCV
#include "opencv2/highgui/highgui.hpp"



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
    vector<Mat> images;
	vector<int> labels;
    string data_path;
    
    cout << "Building face detection model..." << endl;
    cout << "Path to data: ";
    cin >> data_path;
    
	// path to your CSV
	string fn_csv = string(data_path);
	// read in the images
	try {
		read_csv(fn_csv, images, labels);
	} catch(exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\"." << endl;
		exit(1);
	}
    
	// get width and height
	int width = images[0].cols;
	int height = images[0].rows;
    
	// get test instances
	Mat testSample = images[images.size()-1];
	int testLabel = labels[labels.size()-1];
	// ... and delete last element
	images.pop_back();
	labels.pop_back();
    cout << "Data loaded successfully. Please specify number of components: ";
    int num_components = 80; // default
    cin >> num_components;
	// num_components eigenfaces
	// int num_components = 80;
	// compute the eigenfaces
    cout << "Building eigenfaces model..." << endl;
	Eigenfaces eigenfaces(images, labels, num_components);    

    
    
    //
    // RUN FACE TRACKING ON VIDEO FEED
    //
    // display information
    cout << "Press esc to quit program." << endl;
    
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




void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels) {
	ifstream file(filename.c_str(), ifstream::in);
	if(!file)
		throw std::exception();
	string line, path, classlabel;
	// for each line
	while (std::getline(file, line)) {
		// get current line
		stringstream liness(line);
		// split line
		getline(liness, path, ';');
		getline(liness, classlabel);
		// push pack the data
		images.push_back(imread(path,0));
        Mat img = imread(path,0);
		labels.push_back(atoi(classlabel.c_str()));
	}
}






