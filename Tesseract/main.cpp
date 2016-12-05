#include <baseapi.h>
#include <allheaders.h>
#include <WTypes.h>
#include <OleAuto.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>

#include "detectionword.h"

using namespace cv;
using namespace std;
using namespace tesseract;

int main(){

	DetectionWord word;

	word.printDetectionWord("ps12.png");

	return 0;
}