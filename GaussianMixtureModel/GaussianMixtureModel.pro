QT += core
QT -= gui

TARGET = GaussianMixtureModel
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    gaussianmixture.cpp

INCLUDEPATH += /usr/include/opencv \
                   /usr/include/opencv2


LIBS += /usr/lib/libopencv_core.so.3.3.1
LIBS += /usr/lib/libopencv_highgui.so.3.3.1
LIBS += /usr/lib/libopencv_core.so.3.3.1
LIBS += /usr/lib/libopencv_highgui.so.3.3.1
LIBS += /usr/lib/libopencv_calib3d.so.3.3.1
LIBS += /usr/lib/libopencv_features2d.so.3.3.1
LIBS += /usr/lib/libopencv_flann.so.3.3.1
LIBS += /usr/lib/libopencv_imgcodecs.so.3.3.1
LIBS += /usr/lib/libopencv_imgproc.so.3.3.1
LIBS += /usr/lib/libopencv_ml.so.3.3.1
LIBS += /usr/lib/libopencv_objdetect.so.3.3.1
LIBS += /usr/lib/libopencv_video.so.3.3.1
LIBS += /usr/lib/libopencv_dnn.so.3.3.1
LIBS += /usr/lib/libopencv_photo.so.3.3.1
LIBS += /usr/lib/libopencv_shape.so.3.3.1
LIBS += /usr/lib/libopencv_stitching.so.3.3.1
LIBS += /usr/lib/libopencv_superres.so.3.3.1
LIBS += /usr/lib/libopencv_videoio.so.3.3.1
LIBS += /usr/lib/libopencv_videostab.so.3.3.1

#使QT支持C++11
QMAKE_CXXFLAGS += -std=c++11

#CONFIG += c++11

HEADERS += \
    gaussianmixture.h

