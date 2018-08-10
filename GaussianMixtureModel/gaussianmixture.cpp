#include "gaussianmixture.h"

GaussianMixture::GaussianMixture(int _numCluster,cv::Mat Img)
{
    int dims = Img.channels();
    int nSamples = Img.cols*Img.rows;
    cv::Mat ImgPoints(nSamples,dims,CV_64FC1);
    color = new cv::Scalar(_numCluster);
    cv::RNG r;
    for(int i = 0; i < _numCluster; i++)
    {
        color[i] = cv::Scalar(r.uniform(0,255),r.uniform(0,255),r.uniform(0,255));
    }

    for(int i = 0; i < Img.rows; i++)
    {
        unsigned char *ImgData = Img.ptr<unsigned char>(i);
        for(int j = 0 , k = 0; j < Img.cols*Img.channels(); j+=3,k++)
        {
            int index = i * Img.cols + k;
            double *ImgPointsData = ImgPoints.ptr<double>(index);
            ImgPointsData[0] = static_cast<double>(ImgData[j]);
            ImgPointsData[1] = static_cast<double>(ImgData[j + 1]);
            ImgPointsData[2] = static_cast<double>(ImgData[j + 2]);

        }
    }


    _EM = cv::ml::EM::create();
    _EM->setClustersNumber(_numCluster);
    _EM->setTermCriteria(cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT,100,0.1));
    _EM->setCovarianceMatrixType(cv::ml::EM::COV_MAT_DIAGONAL);
    _EM->trainEM(ImgPoints,cv::noArray(),cv::noArray(),cv::noArray());
}



GaussianMixture::GaussianMixture(std::string filePath)
{
    _EM->load(filePath);
}

bool GaussianMixture::SaveTrainModel(std::__cxx11::string filePath)
{
    _EM->save(filePath);
    return true;
}

float GaussianMixture::classification(cv::Mat InputImg,cv::Mat &OutPutImg)
{
    OutPutImg = cv::Mat::zeros(InputImg.size(),CV_8UC3);
    cv::Mat Samples(1,InputImg.channels(),CV_64FC1);


    //cv::Scalar color[3] = {cv::Scalar(255,255,255),cv::Scalar(0,125,0),cv::Scalar(0,0,0)};

    double time = cv::getTickCount();
    for(int i = 0; i < InputImg.rows; i++)
    {
        unsigned char *InputImgData = InputImg.ptr<unsigned char>(i);
        unsigned char *OutPutImgData = OutPutImg.ptr<unsigned char>(i);
        double *SamplesData = Samples.ptr<double>(0);
        for(int j = 0; j < InputImg.cols * InputImg.channels(); j+=3)
        {
            SamplesData[0] = static_cast<double>(InputImgData[j]);
            SamplesData[1] = static_cast<double>(InputImgData[j + 1]);
            SamplesData[2] = static_cast<double>(InputImgData[j + 2]);
            int response = cvRound(_EM->predict2(Samples,cv::noArray())[1]);
            cv::Scalar c = color[response];
            OutPutImgData[j] = c[0];
            OutPutImgData[j + 1] = c[1];
            OutPutImgData[j + 2] = c[2];
        }
    }
    time = (cv::getTickCount() - time)/cv::getTickFrequency()*1000;
    std::cout << "Time(ms) = "<< time <<std::endl;
    return time;
}
