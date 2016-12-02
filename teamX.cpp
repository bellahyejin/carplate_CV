#include "teamX.h"

teamX::teamX(void)
{
	LP_cascade_name = "cascade1.xml"; // xml���� �����ϴ� �κ�.

	// cascade.xml������ ���� �� �з��Ⱑ �����Ǿ�����, �ĺ������� ���̳����� �������� ����.
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

	// ĳ�Ϻ��� �Һ��� �� �ڼ��ϰ� �������� �����̵Ǽ� ������ ��ó�� �κ��� �Һ������� �مf���ϴ�.
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

	lp_cascade.detectMultiScale(grayScale, licensePlate, 1.1,3,0|CASCADE_SCALE_IMAGE,Size(30,30)); // �̺κ��� ���߿� �Ķ���Ͱ� ���� ����. ��ȣ�� ���� �κ�.
	printf("%d\n",licensePlate.size());
	if(licensePlate.size() !=0)
	{
		//groupRectangles(licensePlate,1,0.2);
		Rect r = licensePlate[0]; // �ϴ� ���� ó���� �ĺ������� fRect�� ����.
		fRect.x = r.x;
		fRect.y = r.y;
		fRect.width = r.width;
		fRect.height = r.height;

		// �ĺ����� ��ºκ�(����� �簢��) -> �з����� ���ɿ� ���� 0������ ��û ���� ����.
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

	// ��ȯ��.
}
