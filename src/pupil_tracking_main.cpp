//  EECS481: DrawFree - Pupil Tracking
//  pupil_tracking_main.cpp
//
//
//  Decscription:
//  This file is the main procedure for independently testing the pupil tracking method.  Call this function to generate a window and run pupil tracking on a built in webcam.
//
//
//  Compilation Procedure:
//      1) First, install OpenCV into your libraries
//      2) g++ -o pupil_tracking_main pupil_tracking_main.cpp `pkg-config --cflags --libs opencv`
//
//
//
//
//  Running Main:
//      ./pupil_tracking_main <FLAGS> <OPTIONS>
//
//
//  FLAGS:
//  This file must be run with flags and options for the user, which are specified below:
//
//  -h, help
//      Display help and other information
//
//  -f <pathtofile>, file <pathtofile>
//      Run pupil tracking on pre-recorded video
//
//  -c, camera
//      Run pupil tracking from webcam
//
//


/////////////////////////////////////////////
// HEADER FILES
/////////////////////////////////////////////
#include "pupil_tracking_main.h"
#include "tracking/recording_tracking_main.h"
#include "tracking/camera_tracking_main.h"


/////////////////////////////////////////////
// LIBRARIES
/////////////////////////////////////////////

// C++ Reference
#include <iostream>
#include <stdio.h>
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




/**
 * @function main
 */
int main( int argc, const char** argv )
{
    // validate input
    //  0 = run help script
    //  1 = run pre-recorded file
    //  2 = run webcam
    //  else: error
    int validation = check_validation(argc, argv);
    
    
    switch ( validation ) {
        
        // HELP SCRIPT
        case 0:
            display_help();
            break;
        
        // PRE-RECORDED FILE
        case 1:
            recording_tracking_main(argv[2]);
            break;
        
        // RUN WEBCAM
        case 2:
            camera_tracking_main();
            break;
            
        // ERROR
        default:
            cout << "Input Error: Run the command with -h for information." << endl;
            return 0;
    }
    

    return 0;
}








/**
 * @function check_validation
 */
int check_validation( int argc, const char** argv )
{
    if ( argc == 1 ) {
        return -1;
    }
    string flag = argv[1];
    if ( argc < 2 || argc > 3 ) {
        return -1;
    }
    else if ( argc == 3 ) {
        return 1;
    }
    else if ( argc == 2 && (!flag.compare("-h") || !flag.compare("help")) ) {
        return 0;
    }
    else if ( argc == 2 && (!flag.compare("-c") || !flag.compare("camera")) ) {
        return 2;
    }
    else {
        return -1;
    }
    
}


/**
 * @function display_help
 */
void display_help()
{

    
    cout << "RUNNING PROGRAM:" << endl;
    cout << "\t./pupil_tracking_main <FLAGS> <OPTION>" << endl << endl;
    cout << "FLAGS:" << endl;
    cout << "\tThis file must be run with flags and options for the user, " << endl;
    cout << "\twhich are specified below:" << endl << endl;
    cout << "\t-h, help" << endl;
    cout << "\t\tDisplay help and other information" << endl << endl;
    cout << "\t-f <pathtofile>, file <pathtofile>" << endl;
    cout << "\t\t Run pupil tracking on pre-recorded video" << endl << endl;
    cout << "\t-c, camera" << endl;
    cout << "\t\t Run pupil tracking from webcam" << endl << endl;
    
    
    
    return;
}





