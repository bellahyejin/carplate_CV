//--------------------------------------------------------------------------------------------------
// Write Team Information
// Team #: 
// Memeber's ID(s) and Name(s): 
// Title of the paper: 
//--------------------------------------------------------------------------------------------------

#ifndef _TEAMX_H_
#define _TEAMX_H_

#include "general.h"
#include "opencv2/opencv.hpp"
#include<iostream>
#include<vector>


using namespace std;
using namespace cv;

class teamX
{
	string LP_cascade_name;
	CascadeClassifier lp_cascade;
	vector<Rect> licensePlate;
	Mat grayScale;

public:
	teamX(void);
	~teamX(void);

public:
	void init();
	void run(cv::Mat &fImg, CvRect &fRect);

private:	
};

#endif
