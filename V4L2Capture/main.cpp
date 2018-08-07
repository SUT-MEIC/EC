#include"v4l2capture.h"

int main()
{
    cv::Mat frame;
    V4L2Capture Capture(0);
    while(1)
    {
        frame = Capture.grap();
        cv::imshow("frame",frame);
        if(cv::waitKey(5) == 'q') break;
    }
    return 0;
}

