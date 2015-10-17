#ifndef CVPEDGE_H
#define CVPEDGE_H

#ifdef WIN32
  #ifndef VPBASIC_EXPORTS
    #define __VPBASIC_IMPORTEXPORT __declspec(dllimport)
  #else
    #define __VPBASIC_IMPORTEXPORT __declspec(dllexport) 
  #endif
#else
  #define __VPBASIC_IMPORTEXPORT
#endif

class CvpPoint;

class __VPBASIC_IMPORTEXPORT CvpEdge
{
  public:
    CvpEdge(CvpPoint *oPoint1, CvpPoint *oPoint2);

    bool        isDivided (void);
    CvpPoint*   getPoint1 (void);
    CvpPoint*   getPoint2 (void);
    CvpEdge*    getEdge1  (void);
    CvpEdge*    getEdge2  (void);
    int         divide    (void);

  private:
    CvpPoint    *oPoint1, *oPoint2;
    bool        fIsDivided;
    CvpEdge     *oEdge1, *oEdge2;

};

#endif