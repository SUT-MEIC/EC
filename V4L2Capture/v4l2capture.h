#ifndef V4L2CAPTURE_H
#define V4L2CAPTURE_H

#include<unistd.h>
#include<error.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<pthread.h>
#include<linux/videodev2.h>
#include<sys/mman.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<iostream>
#include<iomanip>
#include<string>

#define CLEAR(x)  memset(&(x),0,sizeof(x));

class V4L2Capture
{
public:
    V4L2Capture(int dev);
    virtual ~V4L2Capture();

    int openDevice();
    int closeDevice();
    int initDevice();
    int startCapture();
    int stopCapture();
    int freeBuffers();
    int getFrame(void**,size_t *);
    int backFrame();

    void v4l2control(uint32_t id,int32_t value);

    cv::Mat grap(void);

protected:
private:
    int initBuffers();

    struct cam_buffers
    {
        void* start;
        unsigned int length;
    };
    char *devName;
    int capW;
    int capH;
    int fd_cam;
    cam_buffers *buffers;
    unsigned int n_buffers;
    int frameIndex;

    struct v4l2_control control_s;
};

#endif // V4L2CAPTURE_H
