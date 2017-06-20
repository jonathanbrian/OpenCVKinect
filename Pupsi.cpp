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

    //! [start]
    dev->start();
    std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
    std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
    //! [start]

    //! [registration setup]
    libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4), depth2ir(512, 424, 4), depth2rgb(1920, 1080 + 2, 4);

    Mat rgbmat, depthmat, depthmatUndistorted, irmat, rgbd, rgbd2, depthIR;

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
	Mat trn;
	flip(irmat/ 4096.0f,trn,1); //flip the image (normally mirrored)
	//threshold( trn, dst, threshold_value, max_BINARY_value,threshold_type );
        cv::imshow("depth", trn);
	cv::imshow("ir", irmat/ 4096.0f);
        //cv::imshow("depth", depthmat / 4096.0f);

        //Extract the contours so that
	vector<vector<Point> > contours;
	
	vector<Vec4i> hierarchy;
  	//vector<vector<Point> > contours_poly( contours.size() );
	
        Mat test;
        trn.convertTo(test, CV_8UC1); //convert var "test" to CV_8UC1 format
	//Canny(test,test
        findContours( test, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Rect> boundRect( contours.size() ); //for rectangle
	vector<Point2f>center( contours.size() ); // for circle
        vector<float>radius( contours.size() ); //for circle
        /*
	  /// Create a window to display results
  	namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  	/// Create Trackbar to choose type of Threshold
  	createTrackbar( trackbar_type,
                  window_name, &threshold_type,
                  max_type, Threshold_Demo );

  	createTrackbar( trackbar_value,
                  window_name, &threshold_value,
                  max_value, Threshold_Demo );

  	/// Call the function to initialize
   	Threshold_Demo( 0, 0 );
*/


        /// Draw contours
	RNG rng(12345);
        Mat drawing = Mat::zeros( test.size(), CV_8UC1 ); // make a matrix full of zeros

        for( size_t i = 0; i< contours.size(); i++ )
      {		
        std::cout << "contour:" << contours[i].size() << std::endl;
        if( contours[i].size() > 100) 
        {	
		//approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
 std::cout << "rectangle:" << boundingRect( Mat(contours[i]) ) << std::endl;
	boundRect[i] = boundingRect( Mat(contours[i]) );
	Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
	//std::cout << "rectangle:" << boundRect[i].tl() << boundRect[i].br() << std::endl;
	rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
	circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
        }
       }

	
        cv::imshow("test", drawing);

        //! [registration]
        //registration->apply(rgb, depth, &undistorted, &registered, true, &depth2rgb);
        registration->apply(depth, ir, &undistorted, &registered, true, &depth2ir);
        //! [registration]

        //cv::Mat(undistorted.height, undistorted.width, CV_32FC1, undistorted.data).copyTo(depthmatUndistorted);
        //cv::Mat(registered.height, registered.width, CV_8UC4, registered.data).copyTo(rgbd);
        //cv::Mat(depth2rgb.height, depth2rgb.width, CV_32FC1, depth2rgb.data).copyTo(rgbd2);
        cv::Mat(depth2ir.height, depth2ir.width, CV_8UC4, depth2ir.data).copyTo(depthIR);

        //cv::imshow("undistorted", depthmatUndistorted / 4096.0f);
        //cv::imshow("registered", rgbd);
        //cv::imshow("depth2RGB", rgbd2 / 4096.0f);
        //cv::imshow("depth2IR", depthIR / 4096.0f);

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


void Threshold_Demo( int, void* )
{
//src_gray=trn;
  /* 0: Binary
     1: Binary Inverted
     2: Threshold Truncated
     3: Threshold to Zero
     4: Threshold to Zero Inverted
   */

  //threshold(src_gray, dst, threshold_value, max_BINARY_value,threshold_type );

  //imshow( window_name, dst );
}

