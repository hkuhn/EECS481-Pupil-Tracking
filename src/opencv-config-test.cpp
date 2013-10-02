//  EECS481: DrawFree - Pupil Tracking
//  opencv-config-test.cpp
//
//  Description:
//  Compile and run this file to make sure you have properly installed OpenCV
//
//
//
//  Compilation Procedure:
//      1) First, install OpenCV into your libraries
//      2) g++ -o opencv-config-test opencv-config-test.cpp `pkg-config --cflags --libs opencv`
//
//


/////////////////////////////////////////////
// LIBRARIES
/////////////////////////////////////////////

// C++ Reference
#include <iostream>
#include <stdio.h>

// OpenCV
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



/////////////////////////////////////////////
// NAMESPACE
/////////////////////////////////////////////
using namespace std;
using namespace cv;



/**
 * @function main
 */
int main( int argc, const char** argv )
{
    
    // retrieve image
    Mat img = imread("media/test.JPG", CV_LOAD_IMAGE_UNCHANGED);
    
    // test if image was properly loaded
    if (img.empty())
    {
        cout << "Error : Image cannot be loaded!" << endl;
        return -1;
    }
    
    namedWindow("Test Window", CV_WINDOW_AUTOSIZE); // create window
    
    imshow("Test Window", img); // show image in window
    
    waitKey(0); //wait indefinitely until key press, then continue
    
    destroyWindow("Test Window"); //destroy the window
    
    
    
    return 0;
}