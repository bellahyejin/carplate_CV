#pragma once;
#include <opencv\cv.h>
#include<math.h>
#include<stdlib.h>
#include <opencv\highgui.h>

using namespace cv;

class lpCore{
public:
	void myImageCopy(Mat src, Mat dst, Point start, Point end);
	Mat preProcessing(Mat src, Mat dst);
	Mat findCandidateLP(Mat src, Mat dst);
	Mat selectCandidateLP(Mat src, Mat preProcImage, Mat dst);

};

void lpCore::myImageCopy(Mat src, Mat dst, Point start, Point end)
{
	for(int y=start.y;y<end.y;y++)
		for(int x=start.x;x<end.x;x++)
		{
			dst.at<Vec3b>(y,x)=src.at<Vec3b>(y,x);
		}
}

vector<Rect> mySelectionSort(vector<Rect> arr)
{
	for(int i=0;i<arr.size();i++)
	{
		Rect temp;
		for(int j=i+1;j<arr.size();j++)
		{
			if(arr[j].tl().x<arr[i].tl().x)
			{
				temp=arr[i];
				arr[i]=arr[j];
				arr[j]=temp;
			}
		}
	}
	return arr;
}

double avgForInterval(double *arr, int size)
{
	double result=0.0;
	int realCount=0;
	if(size<=1) return -1;
	for(int i=0;i<size;i++)
	{
		if(arr[i]<10)	//간격이 10픽셀 미만은 겹친 숫자로 판별
			continue;
		realCount++;
		result=result+arr[i];
	}

	return result/realCount;
}

double stdevForInterval(double *arr, int size)
{
	double result=0.0, temp=0.0;
	int realCount=0;
	if(size<=1) return -1;
	for(int i=0;i<size;i++)
	{
		if(arr[i]<10)	//간격이 10픽셀 미만은 겹친 숫자로 판별
			continue;
		realCount++;
		temp=temp+pow(arr[i]-avgForInterval(arr,size),2);
	}
	temp=temp/(realCount-1);
	result=sqrt(temp);

	return result;
}

Mat lpCore::preProcessing(Mat src, Mat dst)
{
	dst = Mat::zeros(src.size(), CV_8UC3);
	Mat grayImage;
	cvtColor(src,grayImage,CV_BGR2GRAY);
	Canny(grayImage,dst,50,100,3);
	
	return dst;
}

Mat lpCore::findCandidateLP(Mat src, Mat preProceImage)
{
	Mat dst = Mat::zeros(src.size(), CV_8UC3);
	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	Mat cannyImage;

	vector<vector<Point> > contours;  //  Vectors for 'findContours' function.
	vector<Vec4i> hierarchy;

	//cvtColor(src,cannyImage,CV_BGR2GRAY);	
	//Canny(cannyImage,cannyImage,50,100,3);
	
	findContours(preProceImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point());	//윤곽선정보를 contours에 담음.
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
	
    for(int i = 0; i< contours.size(); i++){
		approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));	//윤곽선을 사각형에 담음.
	}

	double ratio=0.0;
	
	for(int i = 0; i< contours.size(); i++){

        ratio = (double) boundRect[i].width / boundRect[i].height;

		//if((ratio > 2.7 && boundRect[i].area()>2200 && boundRect[i].area()<9000)){		//세로 대비 가로 비율이 2.7 이상과 일정 면적을 만족하면 번호판 후보로 선정
		if((ratio > 2.7 && boundRect[i].area()>2200)){
			
            drawContours(drawing, contours, i, Scalar(0,255,255), 1, 8, hierarchy, 0, Point());
            rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255,0,0), 1, 8, 0);

			myImageCopy(src,dst,boundRect[i].tl(),boundRect[i].br());		//선정된 후보를 캔버스에 다시 그림.
        }
    }
	
	imshow("pre",drawing);
	return dst;
}

Mat lpCore::selectCandidateLP(Mat src, Mat preProcImage, Mat dst)
{
	Mat cannyImage;
	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	vector<vector<Point> > contours;  //  Vectors for 'findContours' function.
    vector<Vec4i> hierarchy;

	cannyImage=preProcessing(preProcImage, cannyImage);

	findContours(cannyImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point());
	vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<Rect> boundRect2(contours.size());
	
	contours_poly.resize(contours.size());
	boundRect.resize(contours.size());
	boundRect2.resize(contours.size());  

	for(int i = 0; i< contours.size(); i++){
        approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
    }
	
	int refinery_count=0;
	double ratio=0.0;

	for(int i = 0; i< contours.size(); i++){
        ratio = (double) boundRect[i].height / boundRect[i].width;

        //  Filtering rectangles height/width ratio, and size.
        if((ratio <= 2.5) && (ratio >= 0.5) && (boundRect[i].area() <= 700)&& (boundRect[i].area() > 100)){	//숫자의 윤곽선으로 판단되는 윤곽선 추출

            drawContours(drawing, contours, i, Scalar(0,255,255), 1, 8, hierarchy, 0, Point());		// 정제된 윤관선을 노란색으로 다시 그리는 부분
            rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255,0,0), 1, 8, 0);

            //  Include only suitable rectangles.
            boundRect2[refinery_count] = boundRect[i];	//숫자 윤곽선을 담음
            refinery_count++;
        }
    }
	boundRect2.resize(refinery_count++);
	boundRect2=mySelectionSort(boundRect2);

	double *interVal= new double[boundRect2.size()-1];
	double distance_x, distance_y, gradient;
	double distance_x_avg=0, distance_x_stdev=0;
	int candidateFirstNumber, candidateLastNumber;
	int countRealNumber=0, count=0;

	for(int u=0;u<boundRect2.size();u++)
	{
		rectangle(preProcImage,boundRect2[u].tl(),boundRect2[u].br(),Scalar(255,0,255),1,8,0);
		
		for(int v=u+1;v<boundRect2.size();v++)
		{
			distance_x= abs(boundRect2[u].tl().x-boundRect2[v].tl().x);
			if(distance_x > 200)  //숫자와 숫자사이 간격이 너무 먼 경우 번호판 윤곽선이 아니라고 판단
                break;

			if(distance_x<=0)
				distance_x=1;
			interVal[v-u-1]=abs(boundRect2[v-1].tl().x-boundRect2[v].tl().x);	//숫자사이의 거리를 담음.
			printf("dis[%d]= %f\n",v-u-1,interVal[v-u-1]);

			distance_y= abs(boundRect2[u].tl().y-boundRect2[v].tl().y);
			if(distance_y<0)
				distance_y=1;
			
			gradient= distance_y/distance_x;		
			count++;
			if(gradient<0.25 && interVal[v-u-1]>=10)	//숫자 윤곽선이 수평을 이루고, 겹친 숫자 없는지 판별
			{
				candidateLastNumber=v;
				countRealNumber++;		//카운팅된 숫자 윤곽선의 개수
			}
		}
		distance_x_avg= avgForInterval(interVal,count);
		distance_x_stdev= stdevForInterval(interVal,count);
		printf("stdev: %f\n",distance_x_stdev);
		if((countRealNumber>=6)&&(distance_x_stdev<10) && (distance_x_stdev>0))	//번호판의 개수가 6개이상(겹친 숫자가 존재가능)이고, 숫자사이 간격의 표준편차가 10 이하인 경우 바로 정답
		{
			candidateFirstNumber=u;
			break;
		}else if((countRealNumber>3) && (distance_x_stdev<15) && (distance_x_stdev>0))	//그렇지 못하면 가장 최적의 첫번째 숫자를 선택
		{
			candidateFirstNumber=u;
		}
	}
	delete [] interVal;

	rectangle(preProcImage, boundRect2[candidateFirstNumber].tl(), boundRect2[candidateFirstNumber].br(), Scalar(0,0,255), 2, 8, 0);
	Point licensePlate[2];
	
	licensePlate[0].x=boundRect2[candidateFirstNumber].tl().x-(8-countRealNumber)*10;
	licensePlate[0].y=boundRect2[candidateFirstNumber].tl().y-20;
	licensePlate[1].x=boundRect2[candidateLastNumber].br().x+10;
	licensePlate[1].y=boundRect2[candidateFirstNumber].br().y+20;
	
	dst=src(Rect(licensePlate[0],licensePlate[1]));
	//myImageCopy(src,dst,licensePlate[0],licensePlate[1]);
	printf("Point0: %d %d\n",licensePlate[0].x,licensePlate[0].y);
	printf("Point1: %d %d %d\n",licensePlate[1].x,licensePlate[1].y, countRealNumber);

	//imshow("pre12",preProcImage);
	//imshow("preProcImage",preProcImage);

	return dst;
}