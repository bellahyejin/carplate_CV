#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "lpCore.h"

using namespace cv;

void main()
{
	lpCore mlpCore;
	Mat src=imread("Plates/i1.png",CV_LOAD_IMAGE_COLOR);
	Mat preProcImage, candidateLP, dst=Mat::zeros(src.size(), CV_8UC3);

	preProcImage=mlpCore.preProcessing(src,preProcImage);
	candidateLP= mlpCore.findCandidateLP(src,preProcImage);
	dst= mlpCore.selectCandidateLP(src,candidateLP,dst);
	
	//imshow("src",src);
	imshow("result",dst);
	
	waitKey(0);
}