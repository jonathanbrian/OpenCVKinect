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
	Mat test;
	Mat flipimg;
	int largest_area=0;
	int largest_contour_index=0;
	Rect bounding_rect;

	flip(inputImg, flipimg, 1); //flip the image (normally mirrored)
	flipimg.convertTo(test, CV_8UC1); //convert var "test" to CV_8UC1 format

	Mat dst;

	// Set threshold and maxValue
	double thresh = 150;
	double maxValue = 240;

	GaussianBlur(flipimg,flipimg,Size(5,5),0,0);
	// Binary Threshold
	threshold(flipimg,dst, thresh, maxValue, THRESH_BINARY);

	//cv::imshow("thresh", dst);

	//RETR_EXTERNAL RETR_LIST RETR_CCOMP RETR_TREE
	findContours(dst, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

	vector<Rect> boundRect(contours.size()); //for rectangle
	vector<Point2f> center(contours.size()); // for circle
	vector<float> radius(contours.size()); //for circle
	vector<vector<Vec4i> > conDefect(contours.size());
	vector<vector<Point> >hull(contours.size());

	for( int i = 0; i< contours.size(); i++ ) // iterate through each contour.
	{
		double a=contourArea( contours[i],false);  //  Find the area of contour
		if(a>largest_area)
		{
			largest_area=a;
			largest_contour_index=i;                //Store the index of largest contour
			bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
		}
	}

	/// Draw contours
	RNG rng(12345);

	Mat drawing = Mat::zeros(flipimg.size(), CV_8UC1); // make a matrix full of zeros

	 rectangle(flipimg, bounding_rect,  Scalar(0,255,0),1, 8,0);

	 Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));


	for (size_t i = 0; i < contours.size(); i++) {

		if (contours[i].size() > 250) {

			convexHull( Mat(contours[i]), hull[i], false);



		    if(hull[i].size() > 60 ) // You need more than 3 indices
		    {
		        convexityDefects(contours[i], hull[i], conDefect[i]);
		    }

		}

	}
	drawContours(drawing, contours, largest_contour_index, color, 2, 8, hierarchy, 0,Point());
	drawContours( drawing, hull, largest_contour_index, color, 1, 8, hierarchy, 0, Point() );

	cv::imshow("contours_tree", drawing);
	//cv::imshow("depth", flipimg);

}
