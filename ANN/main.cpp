#include"opencv2/ml/ml.hpp"
#include"opencv2/core/core.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/highgui/highgui.hpp"
#include<iostream>

using namespace cv;


int main()
{

   int Imgcols = 32;   //列像素值
   int Imgrows = 32;    //行像素值

   int ImgNum = 1;    //每类图像数

   int nClass = 3;     //类数

   int nInputDims = 3; //输入层维数
   int noutputDims = 3;//输出层维数

   int nSamples = nClass*Imgcols*Imgrows*ImgNum;



   Ptr<ml::ANN_MLP> model = ml::ANN_MLP::create();

   float trainingInputData[nSamples][nInputDims] = {0.0};

   float labelsData[nSamples][nClass] = {0.0};

   Mat layerSizes = (Mat_<int>(1,4)<<3,7,9,3);

   model->setLayerSizes(layerSizes);

   model->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM);

   model->setTrainMethod(ml::ANN_MLP::BACKPROP,0.1,0.1);

   model->setTermCriteria(TermCriteria(TermCriteria::EPS+TermCriteria::MAX_ITER,5000,0.01));


   int k = 0;
   for(int i = 0; i < nClass; i++)
   {
       std::stringstream Picture;
       Picture << "/home/nvidia/mycode/ANN/Picture/";
       for(int j = 0;j < ImgNum; j++)
       {
           Mat srcImg = imread(Picture.str()+std::to_string(i)+"/"+std::to_string(j)+".jpg");
           Mat trainImg;

           resize(srcImg,trainImg,Size(Imgcols,Imgrows));
           blur(trainImg,trainImg,Size(7,7));
           imshow("trainImg"+std::to_string(i)+std::to_string(j),trainImg);

           for(int m = 0; m < Imgrows; m++)
           {
               uchar *ImgValue = trainImg.ptr<uchar>(m);
               for(int n = 0; n < Imgcols*trainImg.channels(); n += 3)
               {
                   trainingInputData[k][0] = static_cast<float>(ImgValue[n]);
                   trainingInputData[k][1] = static_cast<float>(ImgValue[n + 1]);
                   trainingInputData[k][2] = static_cast<float>(ImgValue[n + 2]);
                   std::cout<<static_cast<float>(ImgValue[n])<<" "<<static_cast<float>(ImgValue[n+1])<<" "<<static_cast<float>(ImgValue[n+2])<<std::endl;
                   std::cout<<trainingInputData[k][0]<<" "<<trainingInputData[k][1]<<" "<<trainingInputData[k][2]<<" "<<" k = "<<k<<std::endl;
                   k++;
               }
           }
           waitKey(1000);
       }
   }


   Mat TrainingInputs(nSamples,nInputDims,CV_32FC1,trainingInputData);
   std::cout<<"Train input Data OK"<<std::endl;

   for(int i = 0; i < nSamples; i++)
   {
           if(i <= Imgcols*Imgrows*ImgNum)
           {
               labelsData[i][0] = 1;
               labelsData[i][1] = 0;
               labelsData[i][2] = 0;
           }
           else if(i <2*Imgcols*Imgrows*ImgNum && i > Imgcols*Imgrows*ImgNum)
           {
               labelsData[i][0] = 0;
               labelsData[i][1] = 1;
               labelsData[i][2] = 0;
           }
           else if(i >= 2*Imgcols*Imgrows*ImgNum)
           {
               labelsData[i][0] = 0;
               labelsData[i][1] = 0;
               labelsData[i][2] = 1;
           }
           std::cout<<"labelsData["<<i<<"]"<<"[0]"<<" = "<<labelsData[i][0]<<" "<<" labelsData["<<i<<"]"<<"[1]"<<" = "<<labelsData[i][1]<<" labelsData["<<i<<"]"<<"[2]"<<" = "<<labelsData[i][2]<<std::endl;
   }

   destroyAllWindows();
   Mat TrainingLables(nSamples,nClass,CV_32FC1,labelsData);
   std::cout<<" labels ok "<<std::endl;

   Ptr<ml::TrainData> trainData = ml::TrainData::create(TrainingInputs,ml::ROW_SAMPLE,TrainingLables);
   model->train(trainData);
   std::cout<<"Train ok"<<std::endl;


   Mat testImg = imread("/home/nvidia/mycode/ANN/test.jpg");
   Mat testResizeImg;
   resize(testImg,testResizeImg,Size(640,480));

   blur(testResizeImg,testResizeImg,Size(3,3));

   Mat sampleImg(1,nInputDims,CV_32FC1);
   std::cout<<sampleImg.rows<<std::endl;
   Mat responseImg;

   Mat resultImg(Size(640,480),CV_8UC3);

   Point MaxLoc;
   double MaxVal = 0;

   for(int i = 0; i < testResizeImg.rows; i++)
   {
       unsigned char *testImgData = testResizeImg.ptr<unsigned char>(i);
       unsigned char *resultImgData = resultImg.ptr<unsigned char>(i);
       for(int j = 0; j < testResizeImg.cols*testResizeImg.channels(); j+=3)
       {
           sampleImg.at<float>(0,0) = testImgData[j];
           sampleImg.at<float>(0,1) = testImgData[j+1];
           sampleImg.at<float>(0,2) = testImgData[j+2];

           model->predict(sampleImg,responseImg);

           minMaxLoc(responseImg,NULL,&MaxVal,NULL,&MaxLoc);
           //std::cout << "value"<<MaxVal<<" "<<MaxLoc.x << std::endl;

           if(MaxLoc.x == 0) //白
           {
               resultImgData[j] = 255;
               resultImgData[j + 1] = 255;
               resultImgData[j + 2] = 255;
           }
           if(MaxLoc.x == 1) //黑
           {
               resultImgData[j] = 0;
               resultImgData[j + 1] = 0;
               resultImgData[j + 2] = 0;
           }
           if(MaxLoc.x == 2) //绿
           {
               resultImgData[j] = 125;
               resultImgData[j + 1] = 255;
               resultImgData[j + 2] = 125;
           }
       }
   }
   std::cout<<"success"<<std::endl;

   imshow("result",resultImg);
   imshow("src",testResizeImg);

   waitKey(0);
   return 0;
}

float sumMatValue(const Mat& InputImg)
{
    float sumValue = 0;
    int r = InputImg.rows;
    int c = InputImg.cols;

    if(InputImg.isContinuous())
    {
        c = r*c;
        r = 1;
    }

    for(int i = 0; i < r; i++)
    {
        const uchar* linePtr = InputImg.ptr<uchar>(i);
        for(int j = 0; j < c; j++)
        {
            sumValue += linePtr[j];
        }
    }
    return sumValue;
}

void calcGradientFeat(const Mat& InputImg,std::vector<float>& feat)
{
    Mat GrayImage;
    cvtColor(InputImg,GrayImage,CV_BGR2GRAY);
    resize(GrayImage,GrayImage,Size(8,16));

    Mat sobelX,sobelY;

    Sobel(GrayImage,sobelX,CV_32FC1,1,0,1);
    Sobel(GrayImage,sobelY,CV_32FC1,0,1,1);

    sobelX = abs(sobelX);
    sobelY = abs(sobelY);

    float totleValueX = sumMatValue(sobelX);
    float totleValueY = sumMatValue(sobelY);

    for(int i = 0; i < GrayImage.rows; i += 4)
    {
        for(int j = 0; j < GrayImage.cols; j += 4)
        {
            Mat subImageX = sobelX(Rect(j,i,4,4));
            feat.push_back(sumMatValue(subImageX) / totleValueX);

            Mat subImageY = sobelY(Rect(j,i,4,4));
            feat.push_back(sumMatValue(subImageY) / totleValueY);
        }
    }
}
