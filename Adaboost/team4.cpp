
#include "team4.h"

team4::team4(void)
{
    //xml ���� �����κ�
    LP_cascade_name = "cascade4.xml";
    LP_cascade_name_short = "cascade_short.xml";
}


team4::~team4(void) { }

void team4::init()
{
	//add Initialization (if needed)
}

void team4::run(cv::Mat &fImg, CvRect &fRect)
{

    Mat preproImg1; // fImg ��ó�� �����ϴ� Mat

    // �з��� ���� ���� Ȯ�� ����.
    int firstChk = 0; // cascade4.xml
    int secondChk = 0; // cascade_short.xml

    // ��ó�� ����
    preproImg1 = imgPreprocessing(fImg);
    //imshow("preprocessing",preproImg1);

    // �з��� xml���� �ε�
    if (!lp_cascade.load(LP_cascade_name)) {
        printf("cascade4.xml file not loading!!\n");
        exit(0);
    }

    if(!lp_cascade_short.load(LP_cascade_name_short)) {
        printf("cascade_short.xml file not loading!\n");
        exit(0);
    }

    // �˰��� ����(�ӻ�� ���..��, adboost �з��� + ��ȣ�� ���� �̿�)
    /* 1. cascade4.xml �з���� preproImg1�� �̿��� �˻�.( ��ü �̹������� �˻� )
     * 2. cascade_short.xml �з���� ª�� ��ȣ�� ��ģ�� �ѹ��� �˻�
     * 3. ��ȣ�� ������ ���� ��ģ�� �ѹ��� �˻�.
     */

    firstChk = lpClassifier(lp_cascade,preproImg1,licensePlate,fRect);
    // cascade4.xml�� ���� �� �� ���.
    if(firstChk == 0) {
        licensePlate.clear();

        secondChk = lpClassifier2(lp_cascade_short, preproImg1, licensePlate, fRect);

        //cascade_short.xml�� ���� ���� ���. ������ ��ȣ�� ã��.
        if (secondChk == 0) {
            printf("check3)\n");
            int initX = 0, initY = 0, initWidth = 50, initHeight = 10;

            fRect.x = initX;
            fRect.y = initY;
            fRect.width = initWidth;
            fRect.height = initHeight;

            Mat src, candidateLP, dst;

            GaussianBlur(fImg, src, Size(3, 3), 1.5);

            candidateLP = findCandidateLP(src);
            dst = selectCandidateLP(src, candidateLP, fRect);
            if (fRect.x ==
                initX && fRect.y == initY && fRect.width == initWidth && fRect.height == initHeight) {
                fImg.copyTo(src);
                candidateLP = findCandidateLP(src);
                dst = selectCandidateLP(src, candidateLP, fRect);
            }
        }

    }


    // ocr�ۼ� ��Ʈ
    // fRect �簢�� ���� �޾Ƽ� ��ȣ�� ���� �� ���� �ν�!


}

// classfier �Լ� 1 : �� ��ȣ�� �� ª�� ��ȣ�� ���� ����. -> ��κ� ���ȣ�Ǹ� ���� ��
int team4::lpClassifier(CascadeClassifier cascade, Mat fImg,vector<Rect> licensePlate, CvRect &fRect)
{
    int chk = 0;
    cascade.detectMultiScale(fImg, licensePlate, 1.1, 4, 0|CASCADE_FIND_BIGGEST_OBJECT , Size(30, 30));
    if (licensePlate.size() != 0) {
    // 4, 0, -8, 0
        printf("check 1) %d\n", licensePlate.size());
        Rect rect = licensePlate[0]; // ó���� �ĺ��� fRect�� ����.
        fRect.x = rect.x+2;
        fRect.y = rect.y+4;
        fRect.width = rect.width +3;
        fRect.height = rect.height-8;
        chk = 1;
    }
    return chk;
}
//classifier �Լ� 2 : ª�� ��ȣ�� ���� ����
int team4::lpClassifier2(CascadeClassifier cascade, Mat fImg,vector<Rect> licensePlate, CvRect &fRect)
{
    int chk = 0;
    cascade.detectMultiScale(fImg, licensePlate, 1.1, 3, 0 | CASCADE_FIND_BIGGEST_OBJECT , Size(30, 30));
    if (licensePlate.size() != 0) {
        printf("check 2) %d\n", licensePlate.size());
        Rect rect = licensePlate[0]; // ó���� �ĺ������� fRect�� ����.
        fRect.x = rect.x+7;
        fRect.y = rect.y+2;
        fRect.width = rect.width -12;
        fRect.height = rect.height-2;
        chk = 1;
    }
    return chk;
}

int team4::myMin(int a, int b)
{
    if(a<b)
        return a;
    else
        return b;
}

int team4::myMax(int a, int b)
{
    if(a>b)
        return a;
    else
        return b;
}

void team4::myImageCopy(Mat src, Mat dst, Point start, Point end)
{
    for(int y=start.y;y<end.y;y++)
        for(int x=start.x;x<end.x;x++)
        {
            dst.at<Vec3b>(y,x)=src.at<Vec3b>(y,x);
        }
}

Mat team4::imgPreprocessing(Mat img)
{
    Mat dst;
    GaussianBlur(img, dst, Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(dst, dst, CV_BGR2GRAY);
    equalizeHist(dst,dst);

    // ĳ�Ϻ��� �Һ��� �� �ڼ��ϰ� �������� �����̵Ǽ� �з��� ������ ��ó�� �κ��� �Һ������� �ٲ�.
    Mat grad_x, grad_y;
    Sobel(dst, grad_x, CV_16S, 1,0,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(grad_x,grad_x);

    Sobel(dst, grad_y, CV_16S, 0,1,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(grad_y,grad_y);

    addWeighted(grad_x,0.5,grad_y,0.5,0,dst);

    return dst;
}

Mat team4::findCandidateLP(Mat src)
{

    Mat dst = Mat::zeros(src.size(), CV_8UC3);
    Mat drawing = Mat::zeros(src.size(), CV_8UC3);
    Mat cannyImage;

    vector<vector<Point>> contours;  //  Vectors for 'findContours' function.
    vector<Vec4i> hierarchy;

    cvtColor(src,cannyImage,CV_BGR2GRAY);
    Canny(cannyImage,cannyImage,100,300,3);

    findContours(cannyImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point());
    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());

    for(int i = 0; i< contours.size(); i++){
        approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i])); // �������� �簢���� ����
    }

    double ratio=0.0;

    for(int i = 0; i< contours.size(); i++){

        ratio = (double) boundRect[i].width / boundRect[i].height;

        // ���� ��� ���κ����� 2.7 �̻�� ���� ������ �����ϸ� ��ȣ�� �ĺ��� ����
        if((ratio > 1.1 && ratio<6.0  && boundRect[i].area()>1000 && boundRect[i].area()<15000)){

            drawContours(drawing, contours, i, Scalar(0,255,255), 1, 8, hierarchy, 0, Point());
            rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255,0,0), 1, 8, 0);

            // ������ �ĺ��� �ٽ� ĵ������ �׸�
            myImageCopy(src,dst,boundRect[i].tl(),boundRect[i].br());
        }
    }

    //imshow("dst",dst);
    //imshow("preProcImage",drawing);
    return dst;
}

vector<Rect> team4::mySelectionSort(vector<Rect> arr)
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

double team4::avgForInterval(double *arr, int size)
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

double team4::stdevForInterval(double *arr, int size)
{
    double result=0.0, temp=0.0;
    int realCount=0;
    if(size<=1) return -1;
    for(int i=0;i<size;i++)
    {
        if(arr[i]<10  || arr[i]>50)	//������ 10�ȼ� �̸��� ��ģ ���ڷ� ��
            continue;
        realCount++;
        temp=temp+pow(arr[i]-avgForInterval(arr,size),2);
    }
    temp=temp/(realCount-1);
    result=sqrt(temp);

    return result;
}

Mat team4::selectCandidateLP(Mat src, Mat preProcImage, CvRect &fRect)
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
        if((ratio <= 3.5) && (ratio >= 1) && (boundRect[i].area() <= 800)&& (boundRect[i].area() > 100)){

            drawContours(drawing, contours, i, Scalar(0,255,255), 1, 8, hierarchy, 0, Point());
            rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255,0,0), 1, 8, 0);

            //  Include only suitable rectangles.
            boundRect2[refinery_count] = boundRect[i];//���� �������� ����.
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
            if(distance_x > 150)  //���ڿ� ���ڻ����� ������ �ʹ� �� ��� ��ȣ�� �������� �ƴ϶�� �Ǵ�.
                break;

            if(distance_x==0)
                distance_x=1;
            interVal[v-u-1]=median_v-median_v_pre;	//���� ������ �Ÿ��� ����.


            distance_y= abs(boundRect2[u].tl().y-boundRect2[v].tl().y);
            if(distance_y<0)
                distance_y=1;

            gradient= distance_y/distance_x;
            count++;
            //printf("U:%d, R%d median: %d, interval[%d]= %f, gradient: %f\n",u,v-1,median_v_pre,v-u-1,interVal[v-u-1],gradient);
            if(gradient<0.25 &&interVal[v-u-1]<=50)	// ���� �������� ������ �̷�� ��ģ ���ڴ� ������ �Ǻ�.
            {
                candidateLastNumber=v;
                countRealNumber++;		//ī���� �� ���� �������� ��
            }
            else if(interVal[v-u-1]>50)
                break;
            else if(gradient>=0.25)
                continue;
        }
        distance_x_avg= avgForInterval(interVal,count);
        distance_x_stdev= stdevForInterval(interVal,count);
        //printf("stdev: %f, countRealNumber: %d\n",distance_x_stdev,countRealNumber);
        //��ȣ���� ������ 6���̻�(��ģ ���� ���� ����)�̰� ���ڻ��� ������ ǥ�������� 15������ ��� �ٷ� ����.
        if((countRealNumber>=6)&&(distance_x_stdev<15) && (distance_x_stdev>=0))
        {
            candidateFirstNumber=u;
            break;
        }
            // �׷��� ���� ��� ���� ������ ù��° ���ڸ� ����.
        else if((countRealNumber>3) && (distance_x_stdev<15) && (distance_x_stdev>=0))
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