#include "v4l2capture.h"

using namespace std;
using namespace cv;

V4L2Capture::V4L2Capture(int dev)
{

    string videoDev = "/dev/video";
    ostringstream os;
    os << dev;
    videoDev += os.str();

    this->devName = const_cast<char*>(videoDev.c_str());
    this->fd_cam = -1;
    this->buffers = NULL;
    this->n_buffers = 0;
    this->frameIndex = -1;
    this->capW = 640;
    this->capH = 480;

    this->openDevice();
    this->initDevice();
    this->startCapture();

}

V4L2Capture::~V4L2Capture()
{
    this->stopCapture();
    this->freeBuffers();
    this->closeDevice();
}

int V4L2Capture::openDevice()
{
    cout << "Video dev : "<< devName << endl;
    fd_cam = open(devName,O_RDWR);
    if(fd_cam < 0)
    {
        perror("Can't open video device");
    }
    return 0;
}

int V4L2Capture::closeDevice()
{
    if(fd_cam > 0)
    {
        int ret = 0;
        if((ret = close(fd_cam)) < 0)
        {
            perror("Can't close video device");
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

int V4L2Capture::initDevice()
{
    int ret;
    struct v4l2_format cam_format;                //设置摄像头的视频制式，帧格式
    struct v4l2_capability cam_cap;               //显示设备信息
    struct v4l2_cropcap cam_cropcap;         //设置摄像头的捕捉能力
    struct v4l2_fmtdesc cam_fmtdesc;          //查询所有支持的格式
    struct v4l2_crop cam_crop;                        //图像缩放

    /*获取摄像头的基本信息*/
    ret = ioctl(fd_cam,VIDIOC_QUERYCAP,&cam_cap);
    if( ret < 0)
    {
        perror("Can't get device informatiion : VIDIOCGCAP");
    }
    //cout<< " Driver Name: "<< cam_cap.driver << endl << " Card Name: " << cam_cap.card <<endl << " Bus info: "<<cam_cap.bus_info<<endl<<"Driver Version: "<< (cam_cap.version >> 16)&0xFF <<" ,"<< (cam_cap.version >> 8)&0xFF<<","<(cam_cap.version & 0xFF) << endl;
        printf(
            "Driver Name:%s\nCard Name:%s\nBus info:%s\nDriver Version:%u.%u.%u\n",
            cam_cap.driver, cam_cap.card, cam_cap.bus_info,
            (cam_cap.version >> 16) & 0XFF, (cam_cap.version >> 8) & 0XFF,
             cam_cap.version & 0XFF);

    /*获取摄像头所有支持的格式*/
    cam_fmtdesc.index = 0;
    cam_fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cout<<  "Support format: "<<endl;
    while(ioctl(fd_cam,VIDIOC_ENUM_FMT,&cam_fmtdesc) != -1)
    {
        cout << endl<< cam_fmtdesc.index + 1 << endl<<cam_fmtdesc.description<<endl;
        cam_fmtdesc.index++;
    }

    /*获取摄像头捕捉能力*/
    cam_cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(0 == ioctl(fd_cam,VIDIOC_CROPCAP,&cam_cropcap))
    {
        cout<<"Default rec: "<<cam_cropcap.defrect.left<<" left: "<<cam_cropcap.defrect.top<<"widith: "<<cam_cropcap.defrect.width<<"height:"<<cam_cropcap.defrect.height;
        /*获得摄像头的窗口取景参数*/
        cam_crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        cam_crop.c = cam_cropcap.defrect;
        if(-1 == ioctl(fd_cam,VIDIOC_S_CROP,&cam_crop))
        {
            cout<< "Can't set crop para"<<endl;
        }
    }
    else
    {
        cout << "Can't set crop para"<<endl;
    }

    /*设置摄像头帧信息*/
    cam_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cam_format.fmt.pix.width = capW;
    cam_format.fmt.pix.height = capH;
    cam_format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    cam_format.fmt.pix.field = V4L2_FIELD_INTERLACED;
    ret = ioctl(fd_cam,VIDIOC_S_FMT,&cam_format);
    if(ret < 0)
    {
        perror("Can't set frame information");
    }
    cout << "Current data format information: "<<"width"<<cam_format.fmt.pix.width<<"height"<<cam_format.fmt.pix.height<<endl;
    ret = initBuffers();
    if(ret < 0)
    {
        perror("Buffers init error");
    }
    return 0;
}

int V4L2Capture::initBuffers()
{
    int ret;
    /*申请帧缓冲*/
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd_cam,VIDIOC_REQBUFS,&req);
    if(ret < 0)
    {
        perror("Request frame bffers failed");
    }
    if(req.count < 2)
    {
        perror("Request frame buffers while insufficient buffer memory");
    }
    buffers = (struct cam_buffers* )calloc(req.count,sizeof(*buffers));
    if(!buffers)
    {
        perror("Out of memory");
    }
    for(n_buffers = 0;n_buffers<req.count;n_buffers++)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);
        //查询序号为n_buffers 的缓冲，得到其物理地址和大小
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        ret = ioctl(fd_cam,VIDIOC_QUERYBUF,&buf);
        if(ret < 0)
        {
            cout << "VIDIOC_QUERYBUF :  "<<n_buffers<<" failed "<<endl;
            return -1;
        }
        buffers[n_buffers].length = buf.length;
        //映射内存
        buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd_cam,buf.m.offset);
        if(MAP_FAILED == buffers[n_buffers].start)
        {
            cout <<"mmap buffer: "<<n_buffers<<"failed"<<endl;
            return -1;
        }
    }
    return 0;
}

int V4L2Capture::startCapture()
{
    unsigned int i;
    for( i = 0;i<n_buffers;i++)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if(-1 == ioctl(fd_cam,VIDIOC_QBUF,&buf))
        {
            cout << "VIDIOC_QBUF: "<<i<<"failed"<<endl;
            return -1;
        }
    }
    enum v4l2_buf_type type;
    type =V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(-1 == ioctl(fd_cam,VIDIOC_STREAMON,&type))
    {
        cout<<"VIDIOC_QBUF buffer "<< i <<"failed"<<endl;
        return -1;
    }
    return 0;
}

int V4L2Capture::stopCapture()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(-1 == ioctl(fd_cam,VIDIOC_STREAMOFF,&type))
    {
        cout<<"VIDIOC_STREAMOFF error"<<endl;
        return -1;
    }
    return 0;
}

int V4L2Capture::freeBuffers()
{
    unsigned int i;
    for(i = 0;i<n_buffers;++i)
    {
        if(-1 == munmap(buffers[i].start,buffers[i].length))
        {
            cout<<"munmap buffers: "<<i<<"faild"<<endl;
            return -1;
        }
        free(buffers);
        return 0;
    }
    return 1;
}

int V4L2Capture::getFrame(void **frame_buf,size_t* len)
{
    struct v4l2_buffer queue_buf;
    CLEAR(queue_buf);
    queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queue_buf.memory = V4L2_MEMORY_MMAP;
    if(-1 == ioctl(fd_cam,VIDIOC_DQBUF,&queue_buf))
    {
        cout << "VIDIOC_DQBUF error"<<endl;
        return -1;
    }
    *frame_buf = buffers[queue_buf.index].start;
    *len = buffers[queue_buf.index].length;
    frameIndex = queue_buf.index;
    return 0;
}


int V4L2Capture::backFrame()
{
    if(frameIndex != -1)
    {
        struct v4l2_buffer queue_buf;
        CLEAR(queue_buf);
        queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        queue_buf.memory = V4L2_MEMORY_MMAP;
        queue_buf.index = frameIndex;
        if(-1 == ioctl(fd_cam,VIDIOC_QBUF,&queue_buf))
        {
            cout<<"VIDIOC_QBUF error"<<endl;
            return -1;
        }
        return 0;
    }
    return -1;
}


void V4L2Capture::v4l2control(uint32_t id,int32_t value)
{
    control_s.value = value;
    control_s.id = id;
    ioctl(fd_cam,VIDIOC_S_CTRL,&control_s);
}

Mat V4L2Capture::grap(void)
{
    Mat image;
    unsigned char *MJPEGframe = NULL;
    unsigned long MJPEGframeSize = 0;
    this->getFrame((void**)&MJPEGframe,(size_t*)&MJPEGframeSize);
    Mat img(this->capW,this->capH,CV_8UC3,MJPEGframe);
    image = imdecode(img,1);
    this->backFrame();
    return image;
}



