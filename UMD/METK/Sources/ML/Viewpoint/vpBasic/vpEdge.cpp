#include "vpPoint.h"
#include "vpEdge.h"

CvpEdge::CvpEdge(CvpPoint* oPoint1, CvpPoint* oPoint2)
{
   this->oPoint1 = oPoint1;
   this->oPoint2 = oPoint2;
   oPoint1->addNeighbour(oPoint2);
   oPoint2->addNeighbour(oPoint1);
   fIsDivided = false;
   oEdge1 = 0;
   oEdge2 = 0;
}

bool CvpEdge::isDivided()
{ 
   return fIsDivided;
}

CvpPoint* CvpEdge::getPoint1 (void)
{
   return oPoint1;
}


CvpPoint* CvpEdge::getPoint2 (void)
{
   return oPoint2;
}

CvpEdge* CvpEdge::getEdge1()
{
   if (!fIsDivided) return 0; 
   return oEdge1;
}

CvpEdge* CvpEdge::getEdge2()
{
   if (!fIsDivided) return 0; 
   return oEdge2;
}

int  CvpEdge::divide()
{
   if (fIsDivided) return 0;
   float x,y,z,x1,x2,y1,y2,z1,z2;
   if (oPoint1->x < oPoint2->x) {x1 = oPoint1->x; x2 = oPoint2->x;} else {x2 = oPoint1->x; x1 = oPoint2->x;}
   if (oPoint1->y < oPoint2->y) {y1 = oPoint1->y; y2 = oPoint2->y;} else {y2 = oPoint1->y; y1 = oPoint2->y;}
   if (oPoint1->z < oPoint2->z) {z1 = oPoint1->z; z2 = oPoint2->z;} else {z2 = oPoint1->z; z1 = oPoint2->z;}
   x = x1 + 0.5f * (x2-x1);
   y = y1 + 0.5f * (y2-y1);
   z = z1 + 0.5f * (z2-z1);
   CvpPoint *oNewPoint = new CvpPoint(x,y,z);
   oEdge1 = new CvpEdge (oPoint1, oNewPoint);
   oEdge2 = new CvpEdge (oNewPoint, oPoint2);
   oPoint1->removeNeighbour(oPoint2);
   oPoint2->removeNeighbour(oPoint1);
   fIsDivided = true;




   //vektor berechnen (startpunkt, endpunkt)
   //länge berechnen
   //länge durch 2
   //mit vektor multiplizieren
   //neuer punkt (startpunkt + vektor)
   return 1;
}
