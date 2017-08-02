//! [headers]
//C
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <time.h>
#include <signal.h>

//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/flann.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
//C++
#include <iostream>
#include <sstream>

//Libfreenect
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
//! [headers]

using namespace std;
using namespace cv;

bool protonect_shutdown = false; // Whether the running application should shut down.

void sigint_handler(int s)
{
  protonect_shutdown = true;
}

int main()
{
    std::cout << "Streaming from Kinect One sensor!" << std::endl;

    //! [context]
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device *dev = 0;
    libfreenect2::PacketPipeline *pipeline = 0;
	libfreenect2::Freenect2Device::Config config;
    //! [context]

    //! [discovery]
    if(freenect2.enumerateDevices() == 0)
    {
        std::cout << "no device connected!" << std::endl;
        return -1;
    }

    string serial = freenect2.getDefaultDeviceSerialNumber();

    std::cout << "SERIAL: " << serial << std::endl;

    if(pipeline)
    {
        //! [open]
        dev = freenect2.openDevice(serial, pipeline);
        //! [open]
    } else {
        dev = freenect2.openDevice(serial);
    }

    if(dev == 0)
    {
        std::cout << "failure opening device!" << std::endl;
        return -1;
    }

    signal(SIGINT, sigint_handler);
    protonect_shutdown = false;

    //! [listeners]
    libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color |
                                                  libfreenect2::Frame::Depth |
                                                  libfreenect2::Frame::Ir);
    libfreenect2::FrameMap frames;

    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);
    //! [listeners]

    //Depth threshold
	config.MinDepth = 0.5f;
    config.MaxDepth = 3.5f;
	dev->setConfiguration(config);

    //! [start]
    dev->start();

    std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
    std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
    //! [start]

    //! [registration setup]
    libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4), depth2rgb(1920, 1080 + 2, 4);

    Mat rgbmat, depthmat, depthmatUndistorted, irmat, rgbd, rgbd2; //from Protonect

    //Background extraction
    Mat frame, fgMaskMOG2;
    //MOG Background subtractor
    Ptr<BackgroundSubtractor> pMOG2;

    //create Background Subtractor objects
    pMOG2 = createBackgroundSubtractorMOG2(500,16,false); //MOG2 approach

    //! [loop start]
    while(!protonect_shutdown)
    {
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

        //! [loop start]
        cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data).copyTo(rgbmat);
        cv::Mat(ir->height, ir->width, CV_32FC1, ir->data).copyTo(irmat);
        cv::Mat(depth->height, depth->width, CV_32FC1, depth->data).copyTo(depthmat);

        //cv::imshow("rgb", rgbmat);
        cv::Mat new_img;
        cv::resize(rgbmat, new_img, cv::Size(rgb->width/2, rgb->height/2));

        //frame = rgbmat/4096.0f;
        frame = new_img;
        //create GUI windows
        namedWindow("Frame");
        namedWindow("FG Mask MOG 2");

        //update the background model
        pMOG2->apply(frame, fgMaskMOG2);

        imshow("Frame", frame);
        imshow("FG Mask MOG 2", fgMaskMOG2);

        int key = cv::waitKey(1);
        protonect_shutdown = protonect_shutdown || (key > 0 && ((key & 0xFF) == 27)); // shutdown on escape
        //! [loop end]

        listener.release(frames);
    }
    //! [loop end]

    //! [stop]
    dev->stop();
    dev->close();
    //! [stop]

    delete registration;

    std::cout << "Streaming Ends!" << std::endl;
    return 0;
}
