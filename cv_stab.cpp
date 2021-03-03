//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\core.hpp>
//#include <opencv\cv.hpp>
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
using namespace cv;
using namespace std;

Mat currImg, prevImg, colorImg, outImg, grayImg, resultImg;
if (!capture.isOpened())
{
    printf("Can not open the video file");
    exit;
}
capture.read(colorImg);
cvtColor(colorImg,grayImg,CV_BGR2GRAY);
currImg = grayImg.clone();
outImg = grayImg.clone();

Size imgSize;
int winSize = 15;
int maxCorners = 400;
double qualityLevel = 0.05;
double minDistance = 5.0;
int blockSize = 3;
double k = 0.04;
IplImage *prevIplImg, *curIplImg;
int fps = 25;
int frameW = 640;
int frameH = 480;
VideoWriter writeOutputVideo ("result.avi", 0,fps,cvSize(colorImg.cols,colorImg.rows),FALSE);
for(;;)
{
    capture.read(colorImg);
    cvtColor(colorImg,grayImg,CV_BGR2GRAY);
    prevImg = currImg.clone();
    currImg = grayImg.clone();

    prevIplImg = cvCloneImage(&(IplImage)prevImg);
    curIplImg = cvCloneImage(&(IplImage)currImg);
    CvSize img_sz = cvGetSize(prevIplImg);

    vector<Point2f>cornerPrev;      
    cornerPrev.reserve(maxCorners);

    vector<Point2f>cornerCur;
    cornerCur.reserve(maxCorners);

    imgSize = prevImg.size();

    goodFeaturesToTrack(prevImg,cornerPrev,maxCorners,qualityLevel,minDistance,Mat(),3,false,0.04);
    //goodFeaturesToTrack(currImg,cornerCur,maxCorners,qualityLevel,minDistance,noArray(),3,false,0.04);

    //cornerSubPix(prevImg,cornerPrev,Size(winSize,winSize),Size(-1,1),TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,20,0.03));
    //cornerSubPix(currImg,cornerCur,Size(winSize,winSize),Size(-1,1),TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,20,0.03));

    //Call Lucas Kanade algorithm
    CvSize pyr_sz = Size (imgSize.width+8, imgSize.height/3);

    vector<uchar>featureFound;
    featureFound.reserve(maxCorners);

    vector<float>featureErrors;
    featureErrors.reserve(maxCorners);    

    //Only work on gray-scale image
    calcOpticalFlowPyrLK(prevImg,currImg,cornerPrev,cornerCur,featureFound,featureErrors,Size(winSize,winSize),3,
                        cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.3),0,0.0001);

    Mat transformMatrix = estimateGlobalMotionLeastSquares(cornerPrev,cornerCur,AFFINE,0);



    //Computes an optimal affine transformation between two 2D point sets.
    //Mat trans = estimateRigidTransform(currImg,prevImg,false);
    //warpAffine(prevImg,outImg,transformMatrix,Size(640,480),INTER_NEAREST|WARP_INVERSE_MAP);

    warpPerspective(prevImg,outImg,transformMatrix,Size(640,480),INTER_NEAREST|WARP_INVERSE_MAP,BORDER_CONSTANT ,0);

    imshow(inputVideo,colorImg);

    //cvtColor(outImg,resultImg,CV_BGR2GRAY);
    writeOutputVideo.write(outImg);
    //imshow(stabilizedVideo,outImg);
    if(waitKey(27) >= 0) break;