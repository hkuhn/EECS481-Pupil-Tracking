// EECS481: DrawFree
//  GazeTracker.cpp
//
//      DESCRIPTION:
//      This c++ class is an implementation of a pupil tracking algorithm, published by Dr. BLANK and
//      further explained by Tristan Hume.  The class has a default constructor, and built in methods
//      to train and search the frame for the pupils' locations.
//
//
//
//
//


/////////////////////////////////////////////
// HEADER FILES
/////////////////////////////////////////////
#include "GazeTracker.h"


/////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////
const string FACES_PATH_DEFAULT = "../data/haarcascade_frontalface_alt.xml";

// Debugging
const bool kPlotVectorField = false;

// Size constants
const int kEyePercentTop = 25;
const int kEyePercentSide = 13;
const int kEyePercentHeight = 30;
const int kEyePercentWidth = 35;

// Preprocessing
const bool kSmoothFaceImage = false;
const float kSmoothFaceFactor = 0.005;

// Algorithm Parameters
const int kFastEyeWidth = 50;
const int kWeightBlurSize = 5;
const bool kEnableWeight = false;
const float kWeightDivisor = 150.0;
const double kGradientThreshold = 5.0;

// Postprocessing
const bool kEnablePostProcess = true;
const float kPostProcessThreshold = 0.97;

// Eye Corner
const bool kEnableEyeCorner = false;




/////////////////////////////////////////////
// CLASSIMP
/////////////////////////////////////////////

// PUBLIC FUNCTIONS
// TRAIN
void GazeTracker::train() {
    //use the haarcascade_frontalface_alt.xml library
	face_cascade.load(FACES_PATH_DEFAULT);
}

void GazeTracker::train(string pathToFaces) {
	//use the haarcascade_frontalface_alt.xml library
	face_cascade.load(pathToFaces);  
}

// RETRIEVE LOCATIONS
// retrieve faces
vector<Rect> GazeTracker::getFaces(Mat grayscaleFrame) {
    vector<Rect> faces;
    //find faces and store them in the vector array
    face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30));
    return faces;
}

// retrieve eyes
vector<vector<Rect> > GazeTracker::getEyes(vector<Rect> faces) {
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

// retrieve pupils
vector<Point> GazeTracker::getPupils(Rect face, vector<Rect> eyes, Mat blurredFrame) {
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



// HELPER FUNCTIONS
bool GazeTracker::rectInImage(Rect rect, Mat image) {
    return rect.x > 0 && rect.y > 0 && rect.x+rect.width < image.cols &&
    rect.y+rect.height < image.rows;
}
bool GazeTracker::inMat(Point p, int rows, int cols) {
    return p.x >= 0 && p.x < cols && p.y >= 0 && p.y < rows;
}
Mat GazeTracker::matrixMagnitude(const Mat &matX, const Mat &matY) {
    Mat mags(matX.rows,matX.cols,CV_64F);
    for (int y = 0; y < matX.rows; ++y) {
        const double *Xr = matX.ptr<double>(y), *Yr = matY.ptr<double>(y);
        double *Mr = mags.ptr<double>(y);
        for (int x = 0; x < matX.cols; ++x) {
            double gX = Xr[x], gY = Yr[x];
            double magnitude = sqrt((gX * gX) + (gY * gY));
            Mr[x] = magnitude;
        }
    }
    return mags;
}
double GazeTracker::computeDynamicThreshold(const Mat &mat, double stdDevFactor) {
    Scalar stdMagnGrad, meanMagnGrad;
    meanStdDev(mat, meanMagnGrad, stdMagnGrad);
    double stdDev = stdMagnGrad[0] / sqrt(mat.rows*mat.cols);
    return stdDevFactor * stdDev + meanMagnGrad[0];
}

// PUPIL FUNCTIONS
Point GazeTracker::unscalePoint(Point p, Rect origSize) {
    float ratio = (((float)kFastEyeWidth)/origSize.width);
    int x = round(p.x / ratio);
    int y = round(p.y / ratio);
    return Point(x,y);
}
void GazeTracker::scaleToFastSize(const Mat &src, Mat &dst) {
    resize(src, dst, cv::Size(kFastEyeWidth,(((float)kFastEyeWidth)/src.cols) * src.rows));    
}
Mat GazeTracker::computeMatXGradient(const Mat &mat) {
    Mat out(mat.rows,mat.cols,CV_64F);
    
    for (int y = 0; y < mat.rows; ++y) {
        const uchar *Mr = mat.ptr<uchar>(y);
        double *Or = out.ptr<double>(y);
        
        Or[0] = Mr[1] - Mr[0];
        for (int x = 1; x < mat.cols - 1; ++x) {
            Or[x] = (Mr[x+1] - Mr[x-1])/2.0;
        }
        Or[mat.cols-1] = Mr[mat.cols-1] - Mr[mat.cols-2];
    }
    
    return out;
}
void GazeTracker::testPossibleCentersFormula(int x, int y, unsigned char weight,double gx, double gy, Mat &out) {
    // for all possible centers
    for (int cy = 0; cy < out.rows; ++cy) {
        double *Or = out.ptr<double>(cy);
        for (int cx = 0; cx < out.cols; ++cx) {
            if (x == cx && y == cy) {
                continue;
            }
            // create a vector from the possible center to the gradient origin
            double dx = x - cx;
            double dy = y - cy;
            // normalize d
            double magnitude = sqrt((dx * dx) + (dy * dy));
            dx = dx / magnitude;
            dy = dy / magnitude;
            double dotProduct = dx*gx + dy*gy;
            dotProduct = max(0.0,dotProduct);
            // square and multiply by the weight
            if (kEnableWeight) {
                Or[cx] += dotProduct * dotProduct * (weight/kWeightDivisor);
            } else {
                Or[cx] += dotProduct * dotProduct;
            }
        }
    }
}
Point GazeTracker::findEyeCenter(Mat face, Rect eye, string debugWindow) {
    Mat eyeROIUnscaled = face(eye);
    Mat eyeROI;
    scaleToFastSize(eyeROIUnscaled, eyeROI);
    // draw eye region
    // rectangle(face,eye,1234);
    //-- Find the gradient
    Mat gradientX = computeMatXGradient(eyeROI);
    Mat gradientY = computeMatXGradient(eyeROI.t()).t();
    //-- Normalize and threshold the gradient
    // compute all the magnitudes
    Mat mags = matrixMagnitude(gradientX, gradientY);
    //compute the threshold
    double gradientThresh = computeDynamicThreshold(mags, kGradientThreshold);
    //double gradientThresh = kGradientThreshold;
    //double gradientThresh = 0;
    //normalize
    for (int y = 0; y < eyeROI.rows; ++y) {
        double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
        const double *Mr = mags.ptr<double>(y);
        for (int x = 0; x < eyeROI.cols; ++x) {
            double gX = Xr[x], gY = Yr[x];
            double magnitude = Mr[x];
            if (magnitude > gradientThresh) {
                Xr[x] = gX/magnitude;
                Yr[x] = gY/magnitude;
            } else {
                Xr[x] = 0.0;
                Yr[x] = 0.0;
            }
        }
    }
    // imshow(debugWindow,gradientX);
    //-- Create a blurred and inverted image for weighting
    
    Mat weight = eyeROI;
    
    //imshow(debugWindow,weight);
    //-- Run the algorithm!
    Mat outSum = cv::Mat::zeros(eyeROI.rows,eyeROI.cols,CV_64F);
    // for each possible center
    for (int y = 0; y < weight.rows; ++y) {
        const unsigned char *Wr = weight.ptr<unsigned char>(y);
        const double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
        for (int x = 0; x < weight.cols; ++x) {
            double gX = Xr[x], gY = Yr[x];
            if (gX == 0.0 && gY == 0.0) {
                continue;
            }
            testPossibleCentersFormula(x, y, Wr[x], gX, gY, outSum);
            x = x + 4;  // increase speed
        }
        y = y + 4;    // increase speed
    }
    // scale all the values down, basically averaging them
    double numGradients = (weight.rows*weight.cols);
    Mat out;
    outSum.convertTo(out, CV_32F,1.0/numGradients);
    //imshow(debugWindow,out);
    //-- Find the maximum point
    Point maxP;
    double maxVal;
    minMaxLoc(out, NULL,&maxVal,NULL,&maxP);
    
    //-- Flood fill the edges
    if(kEnablePostProcess) {
        Mat floodClone;
        //double floodThresh = computeDynamicThreshold(out, 1.5);
        double floodThresh = maxVal * kPostProcessThreshold;
        threshold(out, floodClone, floodThresh, 0.0f, cv::THRESH_TOZERO);
        if(kPlotVectorField) {
            //plotVecField(gradientX, gradientY, floodClone);
            imwrite("eyeFrame.png",eyeROIUnscaled);
        }
        Mat mask = floodKillEdges(floodClone);
        //imshow(debugWindow + " Mask",mask);
        //imshow(debugWindow,out);
        // redo max
        minMaxLoc(out, NULL,&maxVal,NULL,&maxP,mask);
        
    }
    
    if (maxP.y < 10) {
        maxP.x = 0;
        maxP.y = 0;
    }
    
    return unscalePoint(maxP,eye);
}
bool GazeTracker::floodShouldPushPoint(const Point &np, const Mat &mat) {
    return inMat(np, mat.rows, mat.cols);
}
Mat GazeTracker::floodKillEdges(Mat &mat) {
    rectangle(mat, Rect(0,0,mat.cols,mat.rows), 255);
    
    Mat mask(mat.rows, mat.cols, CV_8U, 255);
    queue<Point> toDo;
    toDo.push(Point(0,0));
    while (!toDo.empty()) {
        Point p = toDo.front();
        toDo.pop();
        if (mat.at<float>(p) == 0.0f) {
            continue;
        }
        // add in every direction
        Point np(p.x + 1, p.y); // right
        if (floodShouldPushPoint(np, mat)) toDo.push(np);
        np.x = p.x - 1; np.y = p.y; // left
        if (floodShouldPushPoint(np, mat)) toDo.push(np);
        np.x = p.x; np.y = p.y + 1; // down
        if (floodShouldPushPoint(np, mat)) toDo.push(np);
        np.x = p.x; np.y = p.y - 1; // up
        if (floodShouldPushPoint(np, mat)) toDo.push(np);
        // kill it
        mat.at<float>(p) = 0.0f;
        mask.at<uchar>(p) = 0;
    }
    return mask;
}












