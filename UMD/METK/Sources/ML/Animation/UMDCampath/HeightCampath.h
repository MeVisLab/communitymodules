
//! Class HeightCampath derived from AbstractCampath.
//! \file HeightCampath.h

#ifndef _HEIGHTCAMPATH_
#define _HEIGHTCAMPATH_

#include "UMDCampath.h"

ML_START_NAMESPACE

//! Class HeightCampath derived from AbstractCampath.
class HeightCampath:public AbstractCampath{
  public:
    HeightCampath(ObjMgrCommunicator*,StringField*,kScriptFrameList*,kCamera*,AnimationCache*,
          vector<SbVec3f>*,Vec3fField*,DoubleField*,Vec3fField*,BoolField*,const std::string&);
    ~HeightCampath();
    void internCalcPositions(SbVec3f* bbc,double,double,SbVec3f*,SbVec3f*,SbVec3f*,double,SbVec3f*,SbVec3f&,SbVec3f&,double&);
};
ML_END_NAMESPACE
#endif
