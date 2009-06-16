#include "vpTriangle.h"
#include "vpEdge.h"

CvpTriangle::CvpTriangle (CvpEdge *oEdge1, CvpEdge *oEdge2, CvpEdge *oEdge3, int iLevel, int iWrongE)
{
  this->oEdge1 = oEdge1;
  this->oEdge2 = oEdge2;
  this->oEdge3 = oEdge3;
  this->iLevel = iLevel;
  this->iWrongE = iWrongE;
  fIsDivided = false;
  if ((iContainerLevel == iLevel) && (vTriangleContainer)) vTriangleContainer->push_back(this);
}

CvpTriangle::CvpTriangle (CvpPoint *oPoint1, CvpPoint *oPoint2, CvpPoint *oPoint3, int iLevel)
{
  oEdge1 = new CvpEdge(oPoint1, oPoint2);
  oEdge2 = new CvpEdge(oPoint2, oPoint3);
  oEdge3 = new CvpEdge(oPoint3, oPoint1);
  this->iLevel = iLevel;
//  this->oEdge1 = oEdge1;
//  this->oEdge2 = oEdge2;
//  this->oEdge3 = oEdge3;
//  this->iLevel = iLevel;
  fIsDivided = false;
  if ((iContainerLevel == iLevel) && (vTriangleContainer)) vTriangleContainer->push_back(this);
}

int CvpTriangle::getLevel()
{
  return iLevel;
}

bool CvpTriangle::isDivided()
{
  return fIsDivided;
}

int CvpTriangle::divide()
{
  if (fIsDivided) return 0;
  if (!oEdge1->isDivided()) oEdge1->divide();
  if (!oEdge2->isDivided()) oEdge2->divide();
  if (!oEdge3->isDivided()) oEdge3->divide();

  CvpEdge *oE11 = oEdge1->getEdge1();
	CvpEdge *oE12 = oEdge1->getEdge2();   
	CvpEdge *oE21 = oEdge2->getEdge1();
	CvpEdge *oE22 = oEdge2->getEdge2();
  CvpEdge *oE31 = oEdge3->getEdge1();				//tausche hier
	CvpEdge *oE32 = oEdge3->getEdge2();


  CvpEdge *oETemp;
  if (iWrongE == 1) {oETemp = oE11; oE11 = oE12; oE12 = oETemp;}
  if (iWrongE == 2) {oETemp = oE21; oE21 = oE22; oE22 = oETemp;}
  if (iWrongE == 3) {oETemp = oE31; oE31 = oE32; oE32 = oETemp;}

	CvpPoint *oNewP1;
	CvpPoint *oNewP2;
	CvpPoint *oNewP3;
    
  if (iWrongE == 1) oNewP1 = oE11->getPoint1(); else oNewP1 = oE11->getPoint2();
  if (iWrongE == 2) oNewP2 = oE21->getPoint1(); else oNewP2 = oE21->getPoint2();
  if (iWrongE == 3) oNewP3 = oE31->getPoint1(); else oNewP3 = oE31->getPoint2();

  				//tausche hier
  /*oTri1 = new CvpTriangle(oE11, new CvpEdge(oE11->getPoint2(), oE31->getPoint2()), oE32, iLevel+1);
  oTri2 = new CvpTriangle(oE12, oE21, new CvpEdge(oE21->getPoint2(), oE11->getPoint2()), iLevel+1);
  oTri3 = new CvpTriangle(new CvpEdge(oE31->getPoint2(), oE21->getPoint2()), oE22, oE31, iLevel+1);
  oTri4 = new CvpTriangle(oTri1->getEdge2(), oTri3->getEdge1(), oTri2->getEdge3(), iLevel+1);
*/
  int iWrE1,iWrE2,iWrE3;
  iWrE1 = iWrE2 = iWrE3 = 0;
  if (iWrongE == 1) iWrE1 = iWrE2 = 1;
  if (iWrongE == 2) iWrE2 = iWrE3 = 2;
  if (iWrongE == 3) iWrE3 = iWrE1 = 3;
 
  oTri1 = new CvpTriangle(oE11, new CvpEdge(oNewP1, oNewP3), oE32, iLevel+1, iWrE1);
  oTri2 = new CvpTriangle(oE12, oE21, new CvpEdge(oNewP2, oNewP1), iLevel+1, iWrE2);
  oTri3 = new CvpTriangle(new CvpEdge(oNewP3, oNewP2), oE22, oE31, iLevel+1, iWrE3);
  oTri4 = new CvpTriangle(oTri1->getEdge2(), oTri3->getEdge1(), oTri2->getEdge3(), iLevel+1);

  return 1;
}

void CvpTriangle::divideToLevel(int iToLevel)
{
  if (iLevel == iToLevel) return;
  divide();
  oTri1->divideToLevel(iToLevel);
  oTri2->divideToLevel(iToLevel);
  oTri3->divideToLevel(iToLevel);
  oTri4->divideToLevel(iToLevel);
}

CvpTriangle* CvpTriangle::getTriangle1()
{
  if (fIsDivided) return 0;
  return oTri1;
}

CvpTriangle* CvpTriangle::getTriangle2()
{
  if (fIsDivided) return 0;
  return oTri2;
}

CvpTriangle* CvpTriangle::getTriangle3()
{
  if (fIsDivided) return 0;
  return oTri3;
}

CvpTriangle* CvpTriangle::getTriangle4()
{
  if (fIsDivided) return 0;
  return oTri4;
}

CvpEdge* CvpTriangle::getEdge1()
{
  return oEdge1;
}

CvpEdge* CvpTriangle::getEdge2()
{
  return oEdge2;
}

CvpEdge* CvpTriangle::getEdge3()
{
  return oEdge3;
}

int CvpTriangle::getWrongEdge()
{
  return iWrongE;
}

int CvpTriangle::iContainerLevel = -1;
vector<CvpTriangle*> *CvpTriangle::vTriangleContainer = 0;
