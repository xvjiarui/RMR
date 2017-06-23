#include <iostream>
#include <string>
#include <stdexcept> 
#include "DigitRecognizer.h"
#include "RuneDetector.hpp"


DigitRecognizer::DigitRecognizer()
{

	rng = RNG(12345);

    lowerBound = Scalar(0, 0, 180);
    upperBound = Scalar(255, 255, 255);

    segmentTable = {
    	{119, 0},
    	{18, 1},
    	{94, 2},
    	{91, 3},
    	{58, 4},
    	{107, 5},
    	{111, 6},
    	{82, 7},
    	{127, 8},
    	{123, 9}
    };

    segmentRects = {
    	Rect(Point(5, 0), Point(35, 5)), // 0
    	Rect(Point(5, 0), Point(10, 30)), // 1
    	Rect(Point(30, 0), Point(35, 30)), //2
	    Rect(Point(5, 27), Point(35, 32)), // 3
	    Rect(Point(2, 30), Point(7, 60)), // 4
	    Rect(Point(27, 30), Point(32, 60)), //5
	    Rect(Point(5, 55), Point(35, 60)) // 6
    };

}

void DigitRecognizer::predict(const Mat& inputImg, Rect2f sudokuPanel)
{
	clear();

	Rect2f digitBoardRect = sudokuPanel;
	digitBoardRect.width = sudokuPanel.width / (283.0*3)*90*5;
	digitBoardRect.height = sudokuPanel.height / (173.0*3)*122.0;
	digitBoardRect -= Point2f(sudokuPanel.width / 4, sudokuPanel.height * 0.8);
	Mat img = inputImg(digitBoardRect);
	Mat temp;
	inputImg.copyTo(temp);
	rectangle(temp, digitBoardRect, Scalar(255, 255, 255));
	imshow("inputImg", inputImg);
	imshow("digit_board", img);

    Mat hsvFrame;
    cvtColor(img, hsvFrame, CV_BGR2HSV);
    inRange(hsvFrame, lowerBound, upperBound, hsvFrame);
    hsvFrame.copyTo(img);
    imshow("hsvFrame", hsvFrame);

	vector<vector<Point> > digitContours;
	vector<Vec4i> digitHierarchy;
	findContours( hsvFrame, digitContours, digitHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	vector<vector<Point> > digitContoursPolys;
	vector<Rect> digitBoundRects;
	int digitAvgWidth = 0;
	int digitAvgCount = 0;
	float lowerThreshold = 0.6;
	float upperThreshold = 1.4;
	for ( int i = 0; i < digitContours.size(); i++ ) 
	{
		vector<Point> curDigitContoursPoly;
		approxPolyDP( digitContours.at(i), curDigitContoursPoly, 3, true );
		digitContoursPolys.push_back(curDigitContoursPoly);
		Rect curBoundingRect = boundingRect(Mat(curDigitContoursPoly));
		float ratio = (float) curBoundingRect.width / (float) curBoundingRect.height;	
		if (ratio < 0.5 * upperThreshold && ratio > 0.5 *lowerThreshold)
		{
			digitAvgCount++;
			digitAvgWidth += curBoundingRect.width;
			digitBoundRects.push_back(curBoundingRect);
		}
	}

	if (digitAvgCount)
	{
		digitAvgWidth /= digitAvgCount;
	}
	else return;

	// for (int i = 0; i < digitBoundRects.size(); ++i)
	// {
	// 	if (digitBoundRects.at(i).width < digitBoardRect.width * 0.1)
	// 	{
	// 		digitBoundRects.at(i).width = digitAvgWidth;
	// 		digitBoundRects.at(i) -= Point(digitAvgWidth* 0.7, 0);
	// 	}
	// }

	sort(digitBoundRects.begin(), digitBoundRects.end(), [] (Rect a, Rect b) { return a.x < b.x; });
	for (int i = 0; i < digitBoundRects.size(); ++i)
	{
		Mat curImg = img(digitBoundRects.at(i));
		resize(curImg, curImg, Size(40, 60));
		digitImgs.push_back(curImg);
	}
	cout << "Digit: " << digitImgs.size() << endl;
	for (int i = 0; i < digitImgs.size(); ++i)
	{
		digitLabels.push_back(recognize(digitImgs.at(i)));
		cout << digitLabels.at(i);
	}
	cout << endl;
	for (int i = 0; i < digitBoundRects.size(); i++) {
		rectangle( img, digitBoundRects.at(i), Scalar(255, 255, 255));
	}
	imshow("img", img);
	// for (int i = 0; i < digitBoundRects.size(); ++i)
	// {
	// 	Mat curImg = img(digitSingleRect);
	// 	resize(curImg, curImg, Size(40, 60));
	// 	digitImgs.push_back(curImg);
	// 	digitSingleRect.x += digitSingleRect.width;
	// }
}

DigitRecognizer::~DigitRecognizer()
{
	digitImgs.clear();
	digitLabels.clear();
}


int DigitRecognizer::recognize(const Mat& img)
{
	int ret = 0;
	for (int i = 0; i < segmentRects.size(); ++i)
	{
		ret <<= 1;
		Mat curImg = img(segmentRects.at(i));
		int total = countNonZero(curImg);
		if ((float)total/ (float) segmentRects.at(i).area() > 0.5)
		{
			ret += 1;
		}
	}
	try
	{
		ret = segmentTable[ret];
	}
	catch (out_of_range e)
	{
		cout << "Cannot recognize" << endl;
		return -1;
	}
	return ret;
}


void DigitRecognizer::clear()
{
	digitImgs.clear();
	digitLabels.clear();
}
