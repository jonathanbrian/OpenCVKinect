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
	//Extract the contours so that
	vector<vector<Point> > contours;
	vector<vector<Point> > contours2;
	vector<vector<Point> > contours3;
	vector<vector<Point> > contours4;

	vector<Vec4i> hierarchy;

	Mat test;
	Mat flipimg;

	flip(inputImg, flipimg, 1); //flip the image (normally mirrored)
	flipimg.convertTo(test, CV_8UC1); //convert var "test" to CV_8UC1 format
	findContours(test, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	findContours(test, contours2, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	findContours(test, contours3, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	findContours(test, contours4, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	/// Draw contours
	RNG rng(12345);
	Mat drawing = Mat::zeros(test.size(), CV_8UC1); // make a matrix full of zeros
	Mat drawing2 = Mat::zeros(test.size(), CV_8UC1); // make a matrix full of zeros
	Mat drawing3 = Mat::zeros(test.size(), CV_8UC1); // make a matrix full of zeros
	Mat drawing4 = Mat::zeros(test.size(), CV_8UC1); // make a matrix full of zeros

	vector<Rect> boundRect(contours.size()); //for rectangle
	vector<Point2f> center(contours.size()); // for circle
	vector<float> radius(contours.size()); //for circle

	for (size_t i = 0; i < contours.size(); i++) {
		if (contours[i].size() > 300) {
			//boundRect[i] = boundingRect(Mat(contours[i]));
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
					rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0,
					Point());
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2,
					8, 0);
			circle(drawing, center[i], (int) radius[i], color, 2, 8, 0);
		}

	}

	/////
	for (size_t i = 0; i < contours2.size(); i++) {
		if (contours2[i].size() > 300) {
			//boundRect[i] = boundingRect(Mat(contours2[i]));
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
					rng.uniform(0, 255));
			drawContours(drawing2, contours2, i, color, 2, 8, hierarchy, 0,
					Point());
		}

	}

	for (size_t i = 0; i < contours3.size(); i++) {
		if (contours3[i].size() > 300) {
			//boundRect[i] = boundingRect(Mat(contours3[i]));
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
					rng.uniform(0, 255));
			drawContours(drawing3, contours3, i, color, 2, 8, hierarchy, 0,
					Point());
		}

	}

	for (size_t i = 0; i < contours4.size(); i++) {
		if (contours4[i].size() > 350) {
			boundRect[i] = boundingRect(Mat(contours4[i]));
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
					rng.uniform(0, 255));
			drawContours(drawing4, contours4, i, color, 2, 8, hierarchy, 0,
					Point());
			rectangle(drawing4, boundRect[i].tl(), boundRect[i].br(), color, 2,
					8, 0);
		}

	}

	circle(flipimg, cv::Point(512/2, 424/2), 10, cv::Scalar(255,0,0),CV_FILLED, CV_AA, 0);


	//imshow
	//cv::imshow("contours_external", drawing);
	//cv::imshow("contours_list", drawing2);
	//cv::imshow("contours_ccomp", drawing3);
	cv::imshow("contours_tree", drawing4);
	cv::imshow("depth", flipimg);
}
