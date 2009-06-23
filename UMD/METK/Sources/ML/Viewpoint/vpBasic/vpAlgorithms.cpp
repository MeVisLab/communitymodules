#include "vpAlgorithms.h"

template <class T> const T& min ( const T& a, const T& b ) {
   return (b<a)?a:b;     // or: return comp(b,a)?a:b; for the comp version
}

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
}

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



void QuickSortRec(vector<CvpPoint*> &vPoints, int iLow, int iHigh)
{

   int i=iLow;
   int j=iHigh;
   CvpPoint* t;
   CvpPoint* x;

   x = vPoints[(iLow+iHigh)/2];

   while (i<=j)
   {
      while (vPoints[i]->compareTo(x) == -1) i++;
      while (vPoints[j]->compareTo(x) ==  1) j--;
      if (i<=j)
      {
         t = vPoints[i];
         vPoints[i] = vPoints[j];
         vPoints[j] = t;
         i++;
         j--;
      }
   }
   if (iLow<j) QuickSortRec(vPoints,iLow,j);
   if (i<iHigh) QuickSortRec(vPoints,i,iHigh);
}



void QuickSort(vpListType1 *vpArray, int iLength)
{
   QuickSortRec(vpArray,0,iLength-1);
}

void QuickSort(vector<CvpPoint*> &vPoints)
{
   QuickSortRec(vPoints,0,vPoints.size()-1);
   for (unsigned int i = 0; i < vPoints.size(); i++) vPoints[i]->id = i;
}


