#include "vpDivSphere.h"
#include "vpTriangle.h"
#include "vpEdge.h"
#include "vpPoint.h"

#include <iostream>
using namespace std;

#include <math.h>

CvpDivSphere::CvpDivSphere(float fMx, float fMy, float fMz, float fRadius, int iSubDiv, int iType)
{
   this->fMx = fMx;
   this->fMy = fMy;
   this->fMz = fMz;
   this->fRadius = fRadius;
   this->iSubDiv = iSubDiv;
   CvpPoint::vPointContainer = &vPoints;
   CvpTriangle::iContainerLevel = iSubDiv;
   CvpTriangle::vTriangleContainer = &vTriangles;
   if (iType == 0)
   {
      CvpPoint *oP1 = new CvpPoint(fMx+fRadius,fMy,fMz);
      CvpPoint *oP2 = new CvpPoint(fMx-fRadius,fMy,fMz);
      CvpPoint *oP3 = new CvpPoint(fMx,fMy+fRadius,fMz);
      CvpPoint *oP4 = new CvpPoint(fMx,fMy-fRadius,fMz);
      CvpPoint *oP5 = new CvpPoint(fMx,fMy,fMz+fRadius);
      CvpPoint *oP6 = new CvpPoint(fMx,fMy,fMz-fRadius);
      CvpEdge *oE13 = new CvpEdge(oP1,oP3);
      CvpEdge *oE35 = new CvpEdge(oP3,oP5);
      CvpEdge *oE51 = new CvpEdge(oP5,oP1);
      CvpEdge *oE52 = new CvpEdge(oP5,oP2);
      CvpEdge *oE23 = new CvpEdge(oP2,oP3);
      CvpEdge *oE24 = new CvpEdge(oP2,oP4);
      CvpEdge *oE45 = new CvpEdge(oP4,oP5);
      CvpEdge *oE14 = new CvpEdge(oP1,oP4);
      CvpEdge *oE36 = new CvpEdge(oP3,oP6);
      CvpEdge *oE61 = new CvpEdge(oP6,oP1);
      CvpEdge *oE46 = new CvpEdge(oP4,oP6);
      CvpEdge *oE62 = new CvpEdge(oP6,oP2);
      CvpTriangle *oTri1 = new CvpTriangle(oE13,oE35,oE51,0);
      CvpTriangle *oTri2 = new CvpTriangle(oE23,oE35,oE52,0);
      CvpTriangle *oTri3 = new CvpTriangle(oE24,oE45,oE52,0);
      CvpTriangle *oTri4 = new CvpTriangle(oE14,oE45,oE51,0);
      CvpTriangle *oTri5 = new CvpTriangle(oE13,oE36,oE61,0);
      CvpTriangle *oTri6 = new CvpTriangle(oE23,oE36,oE62,0);
      CvpTriangle *oTri7 = new CvpTriangle(oE24,oE46,oE62,0);
      CvpTriangle *oTri8 = new CvpTriangle(oE14,oE46,oE61,0);
      oTri1->divideToLevel(iSubDiv);
      oTri2->divideToLevel(iSubDiv);
      oTri3->divideToLevel(iSubDiv);
      oTri4->divideToLevel(iSubDiv);
      oTri5->divideToLevel(iSubDiv);
      oTri6->divideToLevel(iSubDiv);
      oTri7->divideToLevel(iSubDiv);
      oTri8->divideToLevel(iSubDiv);
      remesh();
   }
   if (iType == 1)
   {
      float fGold = (float) 1.618033988;
      float a, b;
      a = 2*fRadius/sqrt(1+(fGold*fGold));
      b = fGold*a;
      CvpPoint *oP1 = new CvpPoint(fMx+.5f*b, fMy, fMz+.5f*a);
      CvpPoint *oP2 = new CvpPoint(fMx+.5f*b, fMy, fMz-.5f*a);
      CvpPoint *oP3 = new CvpPoint(fMx-.5f*b, fMy, fMz-.5f*a);
      CvpPoint *oP4 = new CvpPoint(fMx-.5f*b, fMy, fMz+.5f*a);
      CvpPoint *oP5 = new CvpPoint(fMx+.5f*a, fMy-.5f*b, fMz);
      CvpPoint *oP6 = new CvpPoint(fMx+.5f*a, fMy+.5f*b, fMz);
      CvpPoint *oP7 = new CvpPoint(fMx-.5f*a, fMy+.5f*b, fMz);
      CvpPoint *oP8 = new CvpPoint(fMx-.5f*a, fMy-.5f*b, fMz);
      CvpPoint *oP9 = new CvpPoint(fMx, fMy-.5f*a, fMz-.5f*b);
      CvpPoint *oP10 = new CvpPoint(fMx, fMy+.5f*a, fMz-.5f*b);
      CvpPoint *oP11 = new CvpPoint(fMx, fMy+.5f*a, fMz+.5f*b);
      CvpPoint *oP12 = new CvpPoint(fMx, fMy-.5f*a, fMz+.5f*b);

      CvpEdge *oE6_11 = new CvpEdge(oP6,oP11);
      CvpEdge *oE11_1 = new CvpEdge(oP11,oP1);
      CvpEdge *oE1_6  = new CvpEdge(oP1,oP6);
      CvpEdge *oE6_2  = new CvpEdge(oP6,oP2);
      CvpEdge *oE2_1  = new CvpEdge(oP2,oP1);
      CvpEdge *oE2_10 = new CvpEdge(oP2,oP10);
      CvpEdge *oE10_6 = new CvpEdge(oP10,oP6);
      CvpEdge *oE6_7 = new CvpEdge(oP6,oP7);
      CvpEdge *oE7_10 = new CvpEdge(oP7,oP10);
      CvpEdge *oE7_11 = new CvpEdge(oP7,oP11);

      CvpEdge *oE8_12 = new CvpEdge(oP8,oP12);
      CvpEdge *oE12_5 = new CvpEdge(oP12,oP5);
      CvpEdge *oE5_8  = new CvpEdge(oP5,oP8);
      CvpEdge *oE8_9  = new CvpEdge(oP8,oP9);
      CvpEdge *oE9_5  = new CvpEdge(oP9,oP5);
      CvpEdge *oE9_3 = new CvpEdge(oP9,oP3);
      CvpEdge *oE3_8 = new CvpEdge(oP3,oP8);
      CvpEdge *oE8_4 = new CvpEdge(oP8,oP4);
      CvpEdge *oE4_3 = new CvpEdge(oP4,oP3);
      CvpEdge *oE4_12 = new CvpEdge(oP4,oP12);

      CvpEdge *oE12_11 = new CvpEdge(oP12,oP11);
      CvpEdge *oE1_12 = new CvpEdge(oP1,oP12);
      CvpEdge *oE5_1  = new CvpEdge(oP5,oP1);
      CvpEdge *oE5_2  = new CvpEdge(oP5,oP2);
      CvpEdge *oE2_9  = new CvpEdge(oP2,oP9);
      CvpEdge *oE10_9 = new CvpEdge(oP10,oP9);
      CvpEdge *oE3_10 = new CvpEdge(oP3,oP10);
      CvpEdge *oE3_7 = new CvpEdge(oP3,oP7);
      CvpEdge *oE7_4 = new CvpEdge(oP7,oP4);
      CvpEdge *oE11_4 = new CvpEdge(oP11,oP4);





      CvpTriangle *oTri1 = new CvpTriangle (oE6_11,oE11_1,oE1_6,0);
      CvpTriangle *oTri2 = new CvpTriangle (oE1_6,oE6_2,oE2_1,0);
      CvpTriangle *oTri3 = new CvpTriangle (oE6_2,oE2_10,oE10_6,0);
      CvpTriangle *oTri4 = new CvpTriangle (oE10_6,oE6_7,oE7_10,0);
      CvpTriangle *oTri5 = new CvpTriangle (oE6_7,oE7_11,oE6_11,0,3);

      CvpTriangle *oTri6 = new CvpTriangle (oE8_12,oE12_5,oE5_8,0);
      CvpTriangle *oTri7 = new CvpTriangle (oE5_8,oE8_9,oE9_5,0);
      CvpTriangle *oTri8 = new CvpTriangle (oE8_9,oE9_3,oE3_8,0);
      CvpTriangle *oTri9 = new CvpTriangle (oE3_8,oE8_4,oE4_3,0);
      CvpTriangle *oTri10 = new CvpTriangle (oE8_4,oE4_12,oE8_12,0,3);

      CvpTriangle *oTri11 = new CvpTriangle (oE12_11,oE11_1,oE1_12,0);
      CvpTriangle *oTri12 = new CvpTriangle (oE1_12,oE12_5,oE5_1,0);
      CvpTriangle *oTri13 = new CvpTriangle (oE5_1,oE5_2,oE2_1,0,1);
      CvpTriangle *oTri14 = new CvpTriangle (oE5_2,oE2_9,oE9_5,0);
      CvpTriangle *oTri15 = new CvpTriangle (oE2_9,oE2_10,oE10_9,0,1);
      CvpTriangle *oTri16 = new CvpTriangle (oE10_9,oE9_3,oE3_10,0);
      CvpTriangle *oTri17 = new CvpTriangle (oE3_10,oE3_7,oE7_10,0,1);
      CvpTriangle *oTri18 = new CvpTriangle (oE3_7,oE7_4,oE4_3,0);
      CvpTriangle *oTri19 = new CvpTriangle (oE7_4,oE7_11,oE11_4,0,1);
      CvpTriangle *oTri20 = new CvpTriangle (oE11_4,oE4_12,oE12_11,0);


      oTri1->divideToLevel(iSubDiv);
      oTri2->divideToLevel(iSubDiv);
      oTri3->divideToLevel(iSubDiv);
      oTri4->divideToLevel(iSubDiv);
      oTri5->divideToLevel(iSubDiv);

      oTri6->divideToLevel(iSubDiv);
      oTri7->divideToLevel(iSubDiv);
      oTri8->divideToLevel(iSubDiv);
      oTri9->divideToLevel(iSubDiv);
      oTri10->divideToLevel(iSubDiv);

      oTri11->divideToLevel(iSubDiv);
      oTri12->divideToLevel(iSubDiv);
      oTri13->divideToLevel(iSubDiv);
      oTri14->divideToLevel(iSubDiv);
      oTri15->divideToLevel(iSubDiv);
      oTri16->divideToLevel(iSubDiv);
      oTri17->divideToLevel(iSubDiv);
      oTri18->divideToLevel(iSubDiv);
      oTri19->divideToLevel(iSubDiv);
      oTri20->divideToLevel(iSubDiv);

      /*

      CvpEdge *oE1_11 = new CvpEdge(oP1,oP11);
      CvpEdge *oE11_12 = new CvpEdge(oP11,oP12);
      CvpEdge *oE12_1 = new CvpEdge(oP12,oP1);
      CvpEdge *oE12_4 = new CvpEdge(oP12,oP4);
      CvpEdge *oE4_11 = new CvpEdge(oP4,oP11);
      CvpEdge *oE5_12 = new CvpEdge(oP5,oP12);
      CvpEdge *oE12_8 = new CvpEdge(oP12,oP8);
      CvpEdge *oE8_5 = new CvpEdge(oP8,oP5);
      CvpEdge *oE5_9 = new CvpEdge(oP5,oP9);
      CvpEdge *oE9_8 = new CvpEdge(oP9,oP8);

      CvpEdge *oE9_2 = new CvpEdge(oP9,oP2);
      CvpEdge *oE2_5 = new CvpEdge(oP2,oP5);
      CvpEdge *oE1_5 = new CvpEdge(oP1,oP5);




      CvpTriangle *oTri1 = new CvpTriangle (oE1_11,oE11_12,oE12_1,0);
      CvpTriangle *oTri2 = new CvpTriangle (oE4_11,oE11_12,oE12_4,0);
      CvpTriangle *oTri3 = new CvpTriangle (oE12_8,oE8_5,oE5_12,0);
      CvpTriangle *oTri4 = new CvpTriangle (oE9_8,oE8_5,oE5_9,0);
      CvpTriangle *oTri5 = new CvpTriangle (oE9_2,oE2_5,oE5_9,0);
      CvpTriangle *oTri6 = new CvpTriangle (oE1_5,oE5_12,oE12_1,0);

      oTri1->divideToLevel(iSubDiv);
      oTri2->divideToLevel(iSubDiv);
      oTri3->divideToLevel(iSubDiv);
      oTri4->divideToLevel(iSubDiv);
      oTri5->divideToLevel(iSubDiv);*/
      remesh();





   }
}

void CvpDivSphere::remesh()
{
   for (unsigned int i=0; i<vPoints.size(); i++)
   {
      float fPx = vPoints[i]->x;
      float fPy = vPoints[i]->y;
      float fPz = vPoints[i]->z;
      float fVx = fPx-fMx;
      float fVy = fPy-fMy;
      float fVz = fPz-fMz;
      float fLength = sqrt(fVx*fVx+fVy*fVy+fVz*fVz);//+sin(i/100);//(((float)rand()/RAND_MAX));
      float fMult = fRadius/fLength;
      fVx = fVx*fMult;
      fVy = fVy*fMult;
      fVz = fVz*fMult;
      vPoints[i]->x = fMx+fVx;
      vPoints[i]->y = fMy+fVy;
      vPoints[i]->z = fMz+fVz;
   }
}