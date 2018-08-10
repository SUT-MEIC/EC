#include <opencv2/opencv.hpp>
#include <iostream>
#include"gaussianmixture.h"

using namespace cv;
using namespace cv::ml;
using namespace std;

int main() {
    Mat src = imread("/home/nvidia/Pictures/XiaoMai.jpg");
    if (src.empty()) {
        printf("could not load iamge...\n");
        return -1;
    }
    string inputWinTitle = "Input";
    namedWindow(inputWinTitle, CV_WINDOW_AUTOSIZE);
    imshow(inputWinTitle, src);

    GaussianMixture Text(5,src);

    Mat outImg;
    Text.classification(src,outImg);
    imshow("out",outImg);

    waitKey(0);
    return 0;
}