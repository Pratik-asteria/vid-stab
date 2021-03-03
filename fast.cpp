#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std

int main(){
Mat src= imread("lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);
cout << "image size :" << src.rows << " " << src.cols << "\n";

vector<Keypoint> keypointsD;
Ptr<FastFeatureDetector> detector= FastFeatureDetector::create();
vector<Mat> descriptor;

detector->detect(src, keypointsD, Mat());
drawKeypoints(src, keypointsD, src);
imshow("keypoints", src);
waitkey();
return 0;
}
