//------------------------------------------------------------------------------------------------------------
// This source code was implemented by Chun-Su Park at Sejong University.
// Only the Students who take 'Computer Vision System' class in 2016 can use this source code.
//------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "general.h"
#include "teamX.h"

using namespace std;
using namespace cv;

void _tmain(int argc, _TCHAR* argv[])
{
	int y, x, i, numImg=_NUM_IMG;	
	float precision[_NUM_IMG], recall[_NUM_IMG], hmean[_NUM_IMG];
	long tStart, tElapsed=0;
	teamX detector;
		
	for(i=0 ; i<numImg ; i++)
	{
		char iName[100], gName[100];		
		cv::Mat fImg;
		int hh=0, hm=0, mh=0, mm=0; //for counting hit & miss
		CvRect fRect, gRect, dRect; //found Rect, ground truth Rect, donot case Rect		
						
		sprintf(iName,"\LPImages\\i%d.png",i); 
		sprintf(gName,"\LPImages\\g%d.txt",i);
				
		//fImg = imread(iName, CV_LOAD_IMAGE_GRAYSCALE);
		fImg = imread(iName, CV_LOAD_IMAGE_COLOR);

		if(fImg.cols==0 ||  fImg.rows==0)
		{
			printf("Image Loading Fail!!\n");
			exit(EXIT_FAILURE);
		}
				
		if( readGT(gRect,dRect,gName) )
			exit(EXIT_FAILURE);

		tStart=clock();

		detector.init();
		detector.run(fImg,fRect);

		tElapsed+=(long)(clock()-tStart);
		
		for(y=0 ; y<fImg.rows ; y++)
		{
			for(x=0 ; x<fImg.cols ; x++)
			{
				if(inside(gRect,y,x))
				{
					if(inside(fRect,y,x))
						hh++;
					else 
						hm++;
				}else if(outside(dRect,y,x))
				{
					if(inside(fRect,y,x))
						mh++;
					else 
						mm++;
				}
			}
		}

		if(hh==0)
		{
			precision[i] = 0;
			recall[i] = 0;
		}
		else
		{
			precision[i] = (float)hh/(hh+hm);
			recall[i] = (float)hh/(hh+mh);
		}

		if(precision[i]==0 || recall[i]==0)
			hmean[i] = 0;
		else
			hmean[i] = 2.0*precision[i]*recall[i]/(precision[i]+recall[i]);

		printf("[%2d:%d] Precision: %5.4f, Recall: %5.4f, Hmean: %5.4f\n",i,hh>mh,precision[i],recall[i],hmean[i]);
		
		//for display
		cv::rectangle(fImg,fRect,Scalar(50,0,255),5);
		imshow("test",fImg);
		printf("%d, %d, %d, %d\n",fRect.x,fRect.y,fRect.width,fRect.height);
		waitKey(0);
	}

	double pSum=0.0, rSum=0.0, hSum=0.0;

	for(i=0 ; i<numImg ; i++)
	{
		pSum+=precision[i];
		rSum+=recall[i];
		hSum+=hmean[i];
	}

	printf("Average-> Precision: %5.4f, Recall: %5.4f, HMean: %5.4f (Time:%6.2f)\n", pSum/numImg, rSum/numImg, hSum/numImg, tElapsed/CLOCKS_PER_SEC/numImg);
}

bool readGT(CvRect &gRect, CvRect &dRect, char *gName)
{
	ifstream gFile;
	string line;

  gFile.open(gName);

	if(gFile.is_open())
	{
		int temp;

		gFile >> gRect.x;
		gFile >> gRect.y;
		gFile >> temp; gRect.width = temp - gRect.x;
		gFile >> temp; gRect.height = temp - gRect.y;

		gFile >> dRect.x;
		gFile >> dRect.y;
		gFile >> temp; dRect.width = temp - dRect.x;
		gFile >> temp; dRect.height = temp - dRect.y;
	}
	else
	{
		printf("Text Loading Fail!!\n");
		return true;
	}

	return false;
}

bool inside(CvRect rect, int y, int x)
{
	if(y>=rect.y && y<=(rect.y+rect.height) && x>=rect.x && x<=rect.x+rect.width)
		return true;
	else
		return false;
}

bool outside(CvRect rect, int y, int x)
{
	return !inside(rect,y,x);
}