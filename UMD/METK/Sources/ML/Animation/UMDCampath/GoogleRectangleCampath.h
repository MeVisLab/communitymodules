
//! Class GoogleRectangleCampath derived from AbstractCampath.
//! \file GoogleRectangleCampath.h

#ifndef _GOOGLERECTANGLECAMPATH_
#define _GOOGLERECTANGLECAMPATH_

#include "UMDCampath.h"

ML_START_NAMESPACE

//! Class GoogleRectangleCampath derived from AbstractCampath.
class GoogleRectangleCampath:public AbstractCampath{
  public:
    GoogleRectangleCampath(ObjMgrCommunicator*,StringField*,kScriptFrameList*,kCamera*,AnimationCache*,
          vector<SbVec3f>*,Vec3fField*,DoubleField*,Vec3fField*,BoolField*,const std::string&);
    ~GoogleRectangleCampath();
    void internCalcPositions(SbVec3f* bbc,double,double,SbVec3f*,SbVec3f*,SbVec3f*,double,SbVec3f*,SbVec3f&,SbVec3f&,double&);
    double lastHeight;
};
ML_END_NAMESPACE
#endif
