#ifndef _GENERAL_H_
#define _GENERAL_H_

#include "opencv2/core/core.hpp"

#define _NUM_IMG 100
#define _NUM_START 0

bool inside(CvRect rect, int y, int x);
bool outside(CvRect rect, int y, int x);
bool readGT(CvRect &gRect, CvRect &dRect, char* gName);

//add function declarations here

#endif // _GENERAL_H_