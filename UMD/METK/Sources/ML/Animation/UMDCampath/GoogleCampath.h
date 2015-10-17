
//! Class GoogleCampath derived from AbstractCampath.
//! \file GoogleCampath.h

#ifndef _GOOGLECAMPATH_
#define _GOOGLECAMPATH_

#include "UMDCampath.h"

ML_START_NAMESPACE

//! Class GoogleCampath derived from AbstractCampath.
class GoogleCampath:public AbstractCampath{
  public:
    GoogleCampath(ObjMgrCommunicator*,StringField*,kScriptFrameList*,kCamera*,AnimationCache*,
          vector<SbVec3f>*,Vec3fField*,DoubleField*,Vec3fField*,BoolField*,const std::string&);
    ~GoogleCampath();
    void internCalcPositions(SbVec3f* bbc,double,double,SbVec3f*,SbVec3f*,SbVec3f*,double,SbVec3f*,SbVec3f&,SbVec3f&,double&);
};
ML_END_NAMESPACE
#endif
