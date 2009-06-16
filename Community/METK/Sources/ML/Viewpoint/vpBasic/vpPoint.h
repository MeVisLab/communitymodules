#ifndef CVPPOINT_H
#define CVPPOINT_H

#ifndef VPBASIC_EXPORTS
#define __VPBASIC_IMPORTEXPORT __declspec(dllimport)
#else
#define __VPBASIC_IMPORTEXPORT __declspec(dllexport) 
#endif

#pragma warning(disable: 4251)

#include <vector>
using namespace std;

class __VPBASIC_IMPORTEXPORT CvpPoint
{
  public:
    CvpPoint();
    CvpPoint(float x, float y, float z);    
    float x, y, z;
    int id;
    int removeNeighbour (CvpPoint* oPoint);
    void addNeighbour (CvpPoint* oPoint);
    CvpPoint* getNeighbour (int i);
    int getNrOfNeighbours (void);
    int compareTo (CvpPoint* oPoint); 
    static int iNr;
    static vector<CvpPoint*> *vPointContainer;
    //static void setPointContainer (vector<CvpPoint*> *vPointContainer);
  private:
    vector<CvpPoint*> vNeighbours;
    
};

#endif