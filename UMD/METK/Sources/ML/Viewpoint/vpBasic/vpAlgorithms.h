#ifndef VPALGORITHMS__H
#define VPALGORITHMS__H

#ifndef VPBASIC_EXPORTS
#define __VPBASIC_IMPORTEXPORT __declspec(dllimport)
#else
#define __VPBASIC_IMPORTEXPORT __declspec(dllexport) 
#endif

#include "vpPoint.h"
#include <iostream>
#include <vector>
using namespace std;


struct vpListType1
{
	int Id;
	float zValue;
};

__VPBASIC_IMPORTEXPORT void QuickSortRec(int *iArray, int iLow, int iHigh);
__VPBASIC_IMPORTEXPORT void QuickSort(int *iArray, int iLength);
__VPBASIC_IMPORTEXPORT void QuickSortRec(vpListType1 *vpArray, int iLow, int iHigh);
__VPBASIC_IMPORTEXPORT void QuickSortRec(vector<CvpPoint*> &vPoints, int iLow, int iHigh);
__VPBASIC_IMPORTEXPORT void QuickSort(vpListType1 *vpArray, int iLength);
__VPBASIC_IMPORTEXPORT void QuickSort(vector<CvpPoint*> &vPoints);
#endif

