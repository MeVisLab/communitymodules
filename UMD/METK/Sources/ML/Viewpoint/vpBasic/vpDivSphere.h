#ifndef CVPDIVSPHERE_H
#define CVPDIVSPHERE_H

#pragma warning(disable: 4251)

#ifndef VPBASIC_EXPORTS
#define __VPBASIC_IMPORTEXPORT __declspec(dllimport)
#else
#define __VPBASIC_IMPORTEXPORT __declspec(dllexport) 
#endif

#include <vector>
using namespace std;

class CvpPoint;
class CvpTriangle;
class CvpEdge;

class __VPBASIC_IMPORTEXPORT CvpDivSphere
{
  public:
    CvpDivSphere(float fMx, float fMy, float fMz, float fRadius, int iSubDiv, int iType);
  //private:
    vector<CvpPoint*> vPoints;
    vector<CvpEdge*> vEdges;
    vector<CvpTriangle*> vTriangles;
    float fMx,fMy,fMz,fRadius;
    int iSubDiv;
    void remesh(void);
};

#endif