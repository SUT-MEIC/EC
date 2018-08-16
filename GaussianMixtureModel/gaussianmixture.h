#ifndef GAUSSIANMIXTURE_H
#define GAUSSIANMIXTURE_H
#include<iostream>
#include<cmath>
#include<opencv2/opencv.hpp>

class GaussianMixture
{
public:
    GaussianMixture(std::__cxx11::string filePath);
    GaussianMixture(int _numCluster, cv::Mat Img);
    ~GaussianMixture();
    bool SaveTrainModel(std::string filePath);
    float classification(cv::Mat InputImg, cv::Mat &OutPutImg);
    int FastClassification(cv::Mat InputImg,cv::Mat &OutPutImg);
private:
    cv::Ptr<cv::ml::EM> _EM;
    cv::Scalar *color = new cv::Scalar[3];

};

#endif // GAUSSIANMIXTURE_H
