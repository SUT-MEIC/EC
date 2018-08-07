#include"v4l2capture.h"

int main()
{
    cv::Mat frame;
//    V4L2Capture Cap(0);
//    Cap.v4l2control(V4L2_CID_EXPOSURE_AUTO,V4L2_EXPOSURE_MANUAL);  //设置曝光率，先设置为手动曝光，再设置为曝光值
//    Cap.v4l2control(V4L2_CID_EXPOSURE_ABSOLUTE,50);


    cv::VideoCapture Capture(0);
    while(1)
    {
//        frame = Cap.grap();

        Capture>>frame;
        cv::imshow("frame",frame);
        if(cv::waitKey(5) == 'q') break;
    }
    return 0;
}

