#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

int main()
{
    cv::VideoCapture cap("rtsp://192.168.1.250:554/stream0?maxResolutionVertical=720&liveStreamActive=1", cv::CAP_FFMPEG);

    if(!cap.isOpened())
    {   
        std::cout << "Input error\n";
        return -1;
    }

    cv::namedWindow("Video Feed", cv::WINDOW_AUTOSIZE);

    cv::Mat frame;
    for(;;)
    {
        //std::cout << "Format: " << cap.get(CV_CAP_PROP_FORMAT) << "\n";
        cap >> frame;
        cv::imshow("Video Feed", frame);    
        if (cv::waitKey(10) == 27)
        {
            break;
        }
    }   
    cv::destroyAllWindows();
    return 0;
}