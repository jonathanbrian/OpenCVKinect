/*
 * Algorithms.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: ivs
 */

#include "Algorithms.h"

using namespace cv;
using namespace std;

Algorithms::Algorithms() {

}

void Algorithms::createContours(cv::Mat inputImg) {
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Mat test;
	Mat flipimg;

	flip(inputImg, flipimg, 1); //flip the image (normally mirrored)
	//flipimg.convertTo(test, CV_8UC1); //convert var "test" to CV_8UC1 format

	Mat dst;

	// Set threshold and maxValue
	double thresh = 150;
	double maxValue = 245;

	GaussianBlur(flipimg,flipimg,Size(5,5),0,0);
	// Binary Threshold
	threshold(flipimg,dst, thresh, maxValue, THRESH_BINARY);

	//cv::imshow("thresh", dst);

	//RETR_EXTERNAL RETR_LIST RETR_CCOMP
	findContours(dst, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);


	/// Draw contours
	RNG rng(12345);

	Mat drawing = Mat::zeros(flipimg.size(), CV_8UC1); // make a matrix full of zeros

	vector<Rect> boundRect(contours.size()); //for rectangle
	vector<Point2f> center(contours.size()); // for circle
	vector<float> radius(contours.size()); //for circle
	vector<vector<Vec4i> > defects(contours.size());
	vector<vector<Point> >hull(contours.size());

	for( int i = 0; i < contours.size(); i++ )
	{
		convexHull( Mat(contours[i]), hull[i], false );
	}


	for (size_t i = 0; i < contours.size(); i++) {
		if (contours[i].size() > 250) {
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0,Point());
			drawContours( drawing, hull, i, color, 1, 8, hierarchy, 0, Point() );
		    if(hull[i].size() > 60 ) // You need more than 3 indices
		    {
		        convexityDefects(contours[i], hull[i], defects[i]);
		    }
		}

	}



	cv::imshow("contours_tree", drawing);
	//cv::imshow("depth", flipimg);

}
