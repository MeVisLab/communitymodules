#ifndef CVPTRIANGLE_H
#define CVPTRIANGLE_H

#ifdef WIN32
  #ifndef VPBASIC_EXPORTS
    #define __VPBASIC_IMPORTEXPORT __declspec(dllimport)
  #else
    #define __VPBASIC_IMPORTEXPORT __declspec(dllexport) 
  #endif
#else
  #define __VPBASIC_IMPORTEXPORT
#endif

#include <vector>
using namespace std;

class CvpEdge;
class CvpPoint;

class __VPBASIC_IMPORTEXPORT CvpTriangle
{
  public:
    CvpTriangle(CvpEdge *oEdge1, CvpEdge *oEdge2, CvpEdge *oEdge3, int iLevel, int iWrongE = 0);
    CvpTriangle(CvpPoint *oPoint1, CvpPoint *oPoint2, CvpPoint *oPoint3, int iLevel);

    int getLevel();
    bool isDivided(void);
    int divide(void);
    void divideToLevel(int iToLevel);
    CvpEdge* getEdge1(void);
    CvpEdge* getEdge2(void);
    CvpEdge* getEdge3(void);
    
    CvpTriangle* getTriangle1(void);
    CvpTriangle* getTriangle2(void);
    CvpTriangle* getTriangle3(void);
    CvpTriangle* getTriangle4(void);

    int getWrongEdge(void);

    static int iContainerLevel;
    static vector<CvpTriangle*> *vTriangleContainer;
  private:
    CvpEdge *oEdge1, *oEdge2, *oEdge3;
    CvpTriangle *oTri1, *oTri2, *oTri3, *oTri4; 
    int iLevel;
	  int iWrongE;
    bool fIsDivided;
};

#endif