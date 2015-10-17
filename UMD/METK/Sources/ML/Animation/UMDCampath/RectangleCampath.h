
//! Class RectangleCampath derived from AbstractCampath.
//! \file RectangleCampath.h

#ifndef _RECTANGLECAMPATH_
#define _RECTANGLECAMPATH_

#include "UMDCampath.h"

ML_START_NAMESPACE

//! Class RectangleCampath derived from AbstractCampath.
class RectangleCampath:public AbstractCampath{
  public:
    RectangleCampath(ObjMgrCommunicator*,StringField*,kScriptFrameList*,kCamera*,AnimationCache*,
          vector<SbVec3f>*,Vec3fField*,DoubleField*,Vec3fField*,BoolField*,const std::string&);
    ~RectangleCampath();
    void internCalcPositions(SbVec3f* bbc,double,double,SbVec3f*,SbVec3f*,SbVec3f*,double,SbVec3f*,SbVec3f&,SbVec3f&,double&);
    double lastHeight;
};
ML_END_NAMESPACE
#endif
