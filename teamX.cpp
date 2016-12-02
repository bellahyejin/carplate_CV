#include "teamX.h"

teamX::teamX(void)
{
	LP_cascade_name = "cascade1.xml"; // xml파일 수정하는 부분.

	// cascade.xml파일은 비교적 잘 분류기가 생성되었으나, 후보영역이 많이나오는 문제점을 가짐.
}


teamX::~teamX(void)
{
}

void teamX::init()
{
	//add Initialization (if needed)
}

void teamX::run(cv::Mat &fImg, CvRect &fRect)
{

	GaussianBlur(fImg, grayScale, Size(3,3),0,0,BORDER_DEFAULT);
	cvtColor(grayScale, grayScale, CV_BGR2GRAY);
	equalizeHist(grayScale,grayScale);

	// 캐니보다 소벨이 더 자세하게 윤곽선이 추출이되서 윤곽선 전처리 부분은 소벨엣지로 바꿧습니다.
	Mat grad_x, grad_y;
	Sobel(grayScale, grad_x, CV_16S, 1,0,3,1,0,BORDER_DEFAULT);
	convertScaleAbs(grad_x,grad_x);

	Sobel(grayScale, grad_y, CV_16S, 0,1,3,1,0,BORDER_DEFAULT);
	convertScaleAbs(grad_y,grad_y);

	addWeighted(grad_x,0.5,grad_y,0.5,0,grayScale);
	//Canny(grayScale, grayScale, 50, 200);

	if(!lp_cascade.load(LP_cascade_name))
	{
		printf("cascade.xml file not loading!!\n");
		exit(0);
	}

	lp_cascade.detectMultiScale(grayScale, licensePlate, 1.1,3,0|CASCADE_SCALE_IMAGE,Size(30,30)); // 이부분은 나중에 파라미터값 조정 예정. 번호판 고르는 부분.
	printf("%d\n",licensePlate.size());
	if(licensePlate.size() !=0)
	{
		//groupRectangles(licensePlate,1,0.2);
		Rect r = licensePlate[0]; // 일단 제일 처음의 후보영역을 fRect에 넣음.
		fRect.x = r.x;
		fRect.y = r.y;
		fRect.width = r.width;
		fRect.height = r.height;

		// 후보영역 출력부분(노란색 사각형) -> 분류기의 성능에 따라 0개에서 엄청 많이 나옴.
		for(size_t j = 0; j<licensePlate.size(); j++)
		{
			Point x1,x2;
			x1.x = licensePlate[j].x;
			x1.y = licensePlate[j].y;
			x2.x = licensePlate[j].x+licensePlate[j].width;
			x2.y = licensePlate[j].y+licensePlate[j].height;

			rectangle(fImg,x1,x2,cvScalar(0,255,255),3);
		}
	}

	// 반환값.
}
