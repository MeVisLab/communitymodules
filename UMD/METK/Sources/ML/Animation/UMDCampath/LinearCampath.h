
//! Class LinearCampath derived from AbstractCampath.
//! \file LinearCampath.h

#ifndef _LINEARCAMPATH_
#define _LINEARCAMPATH_

#include "UMDCampath.h"

ML_START_NAMESPACE

//! Class LinearCampath derived from AbstractCampath.
class LinearCampath:public AbstractCampath{
  public:
    LinearCampath(ObjMgrCommunicator*,StringField*,kScriptFrameList*,kCamera*,AnimationCache*,
          vector<SbVec3f>*,Vec3fField*,DoubleField*,Vec3fField*,BoolField*,const std::string&);
    ~LinearCampath();
    void internCalcPositions(SbVec3f* bbc,double,double,SbVec3f*,SbVec3f*,SbVec3f*,double,SbVec3f*,SbVec3f&,SbVec3f&,double&);
};
ML_END_NAMESPACE
#endif
