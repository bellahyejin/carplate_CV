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
#include<opencv2/highgui/highgui.hpp>

#include<iostream>
#include<vector>
#include<math.h>
#include<stdlib.h>


using namespace std;
using namespace cv;

class team4
{
	string LP_cascade_name_short;
    string LP_cascade_name;

    CascadeClassifier lp_cascade;
    CascadeClassifier lp_cascade_short;

    vector<Rect> licensePlate;


public:
	team4(void);
	~team4(void);

public:
	void init();
	void run(cv::Mat &fImg, CvRect &fRect);

    int myMin(int a, int b);
    int myMax(int a, int b);
    void myImageCopy(Mat src, Mat dst, Point start, Point end);

	Mat imgPreprocessing(Mat img);
    int lpClassifier(CascadeClassifier cascade, Mat fImg,vector<Rect> licensePlate,CvRect &fRect);
    int lpClassifier2(CascadeClassifier cascade, Mat fImg,vector<Rect> licensePlate, CvRect &fRect);

	vector<Rect> mySelectionSort(vector<Rect> arr);
    double avgForInterval(double *arr, int size);
    double stdevForInterval(double *arr, int size);

	Mat findCandidateLP(Mat src);
    Mat selectCandidateLP(Mat src, Mat preProcImage, CvRect &fRect);

private:
};

#endif
