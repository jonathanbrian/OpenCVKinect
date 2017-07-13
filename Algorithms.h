/*
 * Algorithms.h
 *
 *  Created on: Jun 26, 2017
 *      Author: ivs
 */

#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

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

class Algorithms
{
private:

public:
	Algorithms();
	void createContours(cv::Mat inputImg);

};


#endif /* ALGORITHMS_H_ */
