#pragma once;
#include "teamX.h"
#include <opencv\cv.h>
#include<math.h>
#include<stdlib.h>
#include <opencv\highgui.h>

using namespace cv;

teamX::teamX(void)
{
}


teamX::~teamX(void)
{
}

int myMin(int a, int b)
{
	if(a<b)
		return a;
	else
		return b;
}

int myMax(int a, int b)
{
	if(a>b)
		return a;
	else
		return b;
}

void myImageCopy(Mat src, Mat dst, Point start, Point end)
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
			int median_i=(arr[i].tl().x+arr[i].br().x)/2;
			int median_j=(arr[j].tl().x+arr[j].br().x)/2;
			if(median_j<median_i)
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
		if(arr[i]<10  || arr[i]>50)	//������ 10�ȼ� �̸��� ��ģ ���ڷ� �Ǻ�
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
		if(arr[i]<10  || arr[i]>50)	//������ 10�ȼ� �̸��� ��ģ ���ڷ� �Ǻ�
			continue;
		realCount++;
		temp=temp+pow(arr[i]-avgForInterval(arr,size),2);
	}
	temp=temp/(realCount-1);
	result=sqrt(temp);

	return result;
}

Mat findCandidateLP(Mat src)
{
	Mat dst = Mat::zeros(src.size(), CV_8UC3);
	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	Mat cannyImage;

	vector<vector<Point> > contours;  //  Vectors for 'findContours' function.
	vector<Vec4i> hierarchy;

	cvtColor(src,cannyImage,CV_BGR2GRAY);	
	Canny(cannyImage,cannyImage,100,300,3);

	findContours(cannyImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point());	//������������ contours�� ����.
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());

    for(int i = 0; i< contours.size(); i++){
		approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));	//�������� �簢���� ����.
	}

	double ratio=0.0;

	for(int i = 0; i< contours.size(); i++){

        ratio = (double) boundRect[i].width / boundRect[i].height;

		if((ratio > 1.2 && ratio<6.0  && boundRect[i].area()>1000 && boundRect[i].area()<15000)){		//���� ��� ���� ������ 2.7 �̻�� ���� ������ �����ϸ� ��ȣ�� �ĺ��� ����

            drawContours(drawing, contours, i, Scalar(0,255,255), 1, 8, hierarchy, 0, Point());
            rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255,0,0), 1, 8, 0);

			myImageCopy(src,dst,boundRect[i].tl(),boundRect[i].br());		//������ �ĺ��� ĵ������ �ٽ� �׸�.
        }
    }

	//imshow("preProcImage",drawing);
	return dst;
}

Mat selectCandidateLP(Mat src, Mat preProcImage, CvRect &fRect)
{
	Mat cannyImage;
	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	vector<vector<Point> > contours;  //  Vectors for 'findContours' function.
    vector<Vec4i> hierarchy;

	cvtColor(preProcImage,cannyImage,CV_BGR2GRAY);
	Canny(cannyImage,cannyImage,100,300,3);
	
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
        if((ratio <= 3.0) && (ratio >= 1) && (boundRect[i].area() <= 800)&& (boundRect[i].area() > 100)){	//������ ���������� �ǴܵǴ� ������ ����

            drawContours(drawing, contours, i, Scalar(0,255,255), 1, 8, hierarchy, 0, Point());		// ������ �������� ��������� �ٽ� �׸��� �κ�
            rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255,0,0), 1, 8, 0);

            //  Include only suitable rectangles.
            boundRect2[refinery_count] = boundRect[i];	//���� �������� ����
            refinery_count++;
        }
    }
	if(refinery_count==0)
		return cannyImage;
	boundRect2.resize(refinery_count++);
	boundRect2=mySelectionSort(boundRect2);

	double *interVal= new double[boundRect2.size()-1];
	double distance_x, distance_y, gradient;
	double distance_x_avg=0, distance_x_stdev=0;
	int candidateFirstNumber=0, candidateLastNumber=0;
	int countRealNumber=0, count=0;

	for(int u=0;u<boundRect2.size();u++)
	{
		rectangle(preProcImage,boundRect2[u].tl(),boundRect2[u].br(),Scalar(255,0,255),1,8,0);
		countRealNumber=0, count=0;
		for(int v=u+1;v<boundRect2.size();v++)
		{
			int median_u=(boundRect2[u].tl().x+boundRect2[u].br().x)/2;
			int median_v=(boundRect2[v].tl().x+boundRect2[v].br().x)/2;
			int median_v_pre=(boundRect2[v-1].tl().x+boundRect2[v-1].br().x)/2;

			distance_x= median_v-median_u;
			if(distance_x > 150)  //���ڿ� ���ڻ��� ������ �ʹ� �� ��� ��ȣ�� �������� �ƴ϶�� �Ǵ�
                break;

			if(distance_x==0)
				distance_x=1;
			interVal[v-u-1]=median_v-median_v_pre;	//���ڻ����� �Ÿ��� ����.
			

			distance_y= abs(boundRect2[u].tl().y-boundRect2[v].tl().y);
			if(distance_y<0)
				distance_y=1;
			
			gradient= distance_y/distance_x;		
			count++;
			//printf("U:%d, R%d median: %d, interval[%d]= %f, gradient: %f\n",u,v-1,median_v_pre,v-u-1,interVal[v-u-1],gradient);
			if(gradient<0.25 &&interVal[v-u-1]<=50)	//���� �������� ������ �̷��, ��ģ ���� ������ �Ǻ�
			{
				candidateLastNumber=v;
				countRealNumber++;		//ī���õ� ���� �������� ����
			}
			else if(interVal[v-u-1]>50)
				break;
			else if(gradient>=0.25)
				continue;
		}
		distance_x_avg= avgForInterval(interVal,count);
		distance_x_stdev= stdevForInterval(interVal,count);
		//printf("stdev: %f, countRealNumber: %d\n",distance_x_stdev,countRealNumber);
		if((countRealNumber>=6)&&(distance_x_stdev<15) && (distance_x_stdev>=0))	//��ȣ���� ������ 6���̻�(��ģ ���ڰ� ���簡��)�̰�, ���ڻ��� ������ ǥ�������� 10 ������ ��� �ٷ� ����
		{
			candidateFirstNumber=u;
			break;
		}else if((countRealNumber>3) && (distance_x_stdev<15) && (distance_x_stdev>=0))	//�׷��� ���ϸ� ���� ������ ù��° ���ڸ� ����
		{
			candidateFirstNumber=u;
			break;
		}
	}
	delete [] interVal;

	rectangle(preProcImage, boundRect2[candidateFirstNumber].tl(), boundRect2[candidateFirstNumber].br(), Scalar(0,0,255), 2, 8, 0);
	Point licensePlate[2];
	
	licensePlate[0].x=boundRect2[candidateFirstNumber].tl().x-5;
	licensePlate[0].y=myMin(boundRect2[candidateFirstNumber].tl().y,boundRect2[candidateLastNumber].tl().y)-5;
	licensePlate[1].x=boundRect2[candidateLastNumber].br().x+5;
	licensePlate[1].y=myMax(boundRect2[candidateFirstNumber].br().y,boundRect2[candidateLastNumber].br().y)+5;
	
	fRect.x=licensePlate[0].x;
	fRect.y=licensePlate[0].y;
	fRect.width=licensePlate[1].x-licensePlate[0].x;
	fRect.height=licensePlate[1].y-licensePlate[0].y;
	
	
	Mat dst;
	//dst=Mat(src,Rect(licensePlate[0],licensePlate[1]));
	//imshow("dst",dst);
	
	//myImageCopy(src,dst,licensePlate[0],licensePlate[1]);
	//printf("Point0: %d %d\n",licensePlate[0].x,licensePlate[0].y);
	//printf("Point1: %d %d %d\n",licensePlate[1].x,licensePlate[1].y, countRealNumber);

	//imshow("preProcImage",preProcImage);

	return dst;
}


void teamX::init()
{
	//add Initialization (if needed)
}

void teamX::run(cv::Mat &fImg, CvRect &fRect)
{
	//Find fRect Here
	int initX=0, initY=0, initWidth=50, initHeight=10;

	fRect.x = initX;
	fRect.y = initY;
	fRect.width = initWidth;
	fRect.height =initHeight;

	Mat src, preProcImage, candidateLP, dst;

	float restore_rateX, restore_rateY;
	restore_rateX=fImg.cols/640.0;
	restore_rateY=fImg.rows/480.0;

	//resize(fImg,src,Size(640,480),0,0,CV_INTER_NN);
	GaussianBlur(fImg,src , Size(3, 3), 1.5);

	candidateLP= findCandidateLP(src);
	dst= selectCandidateLP(src,candidateLP,fRect);
	if(fRect.x==initX && fRect.y==initY && fRect.width==initWidth && fRect.height==initHeight)
	{
		fImg.copyTo(src);
		candidateLP= findCandidateLP(src);
		dst= selectCandidateLP(src,candidateLP,fRect);
	}

	printf("testbug tl: %d %d br: %d %d\n",fRect.x,fRect.y,fRect.x+fRect.width,fRect.y+fRect.height);
	//imshow("result",dst);
	
	//waitKey(0);

}
