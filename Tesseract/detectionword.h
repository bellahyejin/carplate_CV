#pragma once

#include <baseapi.h>
#include <allheaders.h>
#include <WTypes.h>
#include <OleAuto.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>

using namespace tesseract;

class DetectionWord{
public:
	DetectionWord(void);
	~DetectionWord(void);
public:
	TessBaseAPI *api;
	void printDetectionWord(char *filename);
private:
	char* UTF8ToANSI(char *changeStr);
};

DetectionWord::DetectionWord(void){
	
}
DetectionWord::~DetectionWord(void){

}

char* DetectionWord::UTF8ToANSI(char *pszCode)
{
	BSTR    bstrWide;
	char*   pszAnsi;
	int     strlength;
	// Get nLength of the Wide Char buffer
	strlength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1,
		NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, strlength);
	// Change UTF-8 to Unicode (UTF-16)
	MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, 
		strlength);
	// Get nLength of the multi byte buffer 
	strlength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[strlength];
	// Change from unicode to mult byte
	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, strlength, NULL, NULL);
	SysFreeString(bstrWide);
	return pszAnsi;
}

void DetectionWord::printDetectionWord( char *filename){
	char *outText;
	int i = 0;
	char *temp_s;
	api = new TessBaseAPI();

	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng+kor")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	// Open input image with leptonica library
	Pix *image = pixRead(filename);


	api->SetImage(image);
	// Get OCR result
	outText = UTF8ToANSI(api->GetUTF8Text());

	puts(outText);
	//printf("OCR output:\n%s", outText);

	// Destroy used object and release memory
	api->End();
	//delete outText;

	pixDestroy(&image);
}
