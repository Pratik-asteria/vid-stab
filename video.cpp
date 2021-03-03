#include <iostream>
#include "opencv2/core.hpp"    // Basic OpenCV structures (cv::Mat)
#include "opencv2/videoio.hpp"  // Video write
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;



int main(){
	VideoCapture cap("sample_mp4.mp4");
	if(!cap.isOpened()){
		cout<< "error reading the stream" << endl ;
		return -1;
	}
	int frame_width = cap.get(CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);

	CvVideoWrite video("output.mp4", CV_FOURCC('P','I','M','1'), 10, size(frame_width, frame_height));

	while(1){
		Mat frame;
		cap >> frame;
		video.write(frame);
		imshow("frame", frame);

		char c= (char) waitkey(25);
		if(c=27)
		break;
		}

	cap.release();
	video.release();
	destroyAllWondow();
	return 0;

}


		
