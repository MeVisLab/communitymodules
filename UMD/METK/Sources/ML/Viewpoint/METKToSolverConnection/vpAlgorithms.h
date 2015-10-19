#ifndef VPALGORITHMS__H
#define VPALGORITHMS__H
#include <iostream>

using namespace std;

struct vpListType1
{
	int Id;
	float zValue;
};


void QuickSortRec(int *iArray, int iLow, int iHigh)
{

	int i=iLow;
	int j=iHigh;
	int t;
	
	int x=iArray[(iLow+iHigh)/2];

	while (i<=j)
	{
		while (iArray[i]<x) i++;
		while (iArray[j]>x) j--;
		if (i<=j)
		{
			t=iArray[i];
			iArray[i]=iArray[j];
			iArray[j]=t;
			i++;
			j--;
		}
	}
	if (iLow<j) QuickSortRec(iArray,iLow,j);
	if (i<iHigh) QuickSortRec(iArray,i,iHigh);
}

void QuickSort(int *iArray, int iLength)
{
	QuickSortRec(iArray,0,iLength-1);
};

void QuickSortRec(vpListType1 *vpArray, int iLow, int iHigh)
{

	int i=iLow;
	int j=iHigh;
	vpListType1 t;
	
	float x=vpArray[(iLow+iHigh)/2].zValue;

	while (i<=j)
	{
		while (vpArray[i].zValue<x) i++;
		while (vpArray[j].zValue>x) j--;
		if (i<=j)
		{
			t=vpArray[i];
			vpArray[i].Id=vpArray[j].Id;
			vpArray[i].zValue=vpArray[j].zValue;
			
			vpArray[j]=t;
			i++;
			j--;
		}
	}
	if (iLow<j) QuickSortRec(vpArray,iLow,j);
	if (i<iHigh) QuickSortRec(vpArray,i,iHigh);
}




void QuickSort(vpListType1 *vpArray, int iLength)
{
	QuickSortRec(vpArray,0,iLength-1);
};



#endif

