//! [headers]
#include <iostream>// for standard I/O
#include <stdio.h>
#include <iomanip>// for controlling float print precision
#include <time.h>
#include <signal.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include <opencv2/flann.hpp>

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

#include "Algorithms.h"
//! [headers]

using namespace std;
using namespace cv;

/*
 /// Global variables

 int threshold_value = 0;
 int threshold_type = 3;;
 int const max_value = 255;
 int const max_type = 4;
 int const max_BINARY_value = 255;

 Mat src, src_gray, dst;
 char* window_name = "Threshold Demo";

 char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
 char* trackbar_value = "Value";

 /// Function headers
 void Threshold_Demo( int, void* );
 */

bool protonect_shutdown = false; // Whether the running application should shut down.

void sigint_handler(int s) {
	protonect_shutdown = true;
}

int main() {
	std::cout << "Streaming from Kinect One sensor!" << std::endl;

	//! [context]
	libfreenect2::Freenect2 freenect2;
	libfreenect2::Freenect2Device *dev = 0;
	libfreenect2::PacketPipeline *pipeline = 0;
	libfreenect2::Freenect2Device::Config config;
	//! [context]

	//! [discovery]
	if (freenect2.enumerateDevices() == 0) {
		std::cout << "no device connected!" << std::endl;
		return -1;
	}

	string serial = freenect2.getDefaultDeviceSerialNumber();

	std::cout << "SERIAL: " << serial << std::endl;

	if (pipeline) {
		//! [open]
		dev = freenect2.openDevice(serial, pipeline);

		//! [open]
	} else {
		dev = freenect2.openDevice(serial);
	}

	if (dev == 0) {
		std::cout << "failure opening device!" << std::endl;
		return -1;
	}

	signal(SIGINT, sigint_handler);
	protonect_shutdown = false;

	//! [listeners]
	libfreenect2::SyncMultiFrameListener listener(
			libfreenect2::Frame::Color | libfreenect2::Frame::Depth
					| libfreenect2::Frame::Ir);
	libfreenect2::FrameMap frames;

	dev->setColorFrameListener(&listener);
	dev->setIrAndDepthFrameListener(&listener);
	//! [listeners]

	//! [start]
	dev->start();
	std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
	std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
	//! [start]

	//! [registration setup]
	libfreenect2::Registration* registration = new libfreenect2::Registration(
			dev->getIrCameraParams(), dev->getColorCameraParams());
	libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4),
			depth2ir(512, 424, 4), depth2rgb(1920, 1080 + 2, 4);

	Mat rgbmat, depthmat, depthmatUndistorted, irmat, rgbd, rgbd2, depthIR;

	//config.MinDepth = 1.0f;
		//config.MaxDepth = 4.5f;
		//dev->setConfiguration(config);


	std::cout << "max depth :" << config.MaxDepth << std::endl;
	//! [loop start]
	std::cout << "just outside while loop :" << protonect_shutdown << std::endl;
	while (!protonect_shutdown) {
		std::cout << "in while loop :" << protonect_shutdown << std::endl;

		listener.waitForNewFrame(frames);

		std::cout << "after waiting for a frame" << std::endl;

		libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
		libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
		libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];
		//! [loop start]

		cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data).copyTo(rgbmat);
		cv::Mat(ir->height, ir->width, CV_32FC1, ir->data).copyTo(irmat);
		cv::Mat(depth->height, depth->width, CV_32FC1, depth->data).copyTo(
				depthmat);

		std::cout << "after getting images" << std::endl;

		Algorithms myAlgorithms = Algorithms();

		myAlgorithms.createContours(depthmat / 4096.0f);

		//cv::imshow("ir", irmat / 4096.0f);
	    cv::imshow("depth", depthmat / 4096.0f);

		//Extract the contours so that
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		vector<Rect> boundRect(contours.size()); //for rectangle
		vector<Point2f> center(contours.size()); // for circle
		vector<float> radius(contours.size()); //for circle


		//! [registration]
		registration->apply(rgb, depth, &undistorted, &registered, true, &depth2rgb);
		//registration->apply(depth, ir, &undistorted, &registered, true,
				//&depth2ir);

		float x, y, z;
		registration->getPointXYZ(&undistorted, 512/2, 424/2, x,y,z);
		//! [registration]
		//std::cout << "x: " << x << std::endl;
		//std::cout << "y: " << y << std::endl;
		std::cout << "z: " << z << std::endl;


		cv::Mat(depth2ir.height, depth2ir.width, CV_8UC4, depth2ir.data).copyTo(
				depthIR);

		int key = cv::waitKey(1);
		protonect_shutdown = protonect_shutdown || (key > 0 && ((key & 0xFF)
				== 27)); // shutdown on escape

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

