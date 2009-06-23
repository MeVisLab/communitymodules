#ifndef _UMDCAMPATH_
#define _UMDCAMPATH_

#include <vector>
#include <string>
#include "METK.h"
#include "AnimationParser/kScriptFrameEntry.h"
#include "AnimationParser/kScriptFrameList.h"
#include "kCamera.h"
#include <ObjMgr/mlObjMgrClient.h>
#include "ObjMgrCommunicator.h"
#include "UMDAnimation2/AnimationCache.h"

ML_START_NAMESPACE
struct ScenePosition
{
  vec3 position;
  double height;
  vec4 orientation;
  bool polarCoord;
  std::vector<std::string> objects;
  ScenePosition(vec3 p, double h, vec4 o,bool pc,std::vector<std::string> objs):position(p),height(h),orientation(o),polarCoord(pc),objects(objs){}
  ScenePosition(){}
};

class AbstractCampath
{
  public:
    AbstractCampath(ObjMgrCommunicator*,StringField*,kScriptFrameList*,kCamera*,AnimationCache*,
      vector<SbVec3f>*,Vec3fField*,DoubleField*,Vec3fField*,BoolField*,const std::string&);
    virtual ~AbstractCampath()=0;
    void preCalcCamPathPositions(kScriptFrameEntry* frameEntry, int globalFrameNr, int /*commandID*/);
    virtual void calcPositions(SbVec3f* bbc,kScriptFrameEntry*,int,double,double,SbVec3f*,SbVec3f*,SbVec3f*,SbVec3f*,int frameFrom,int frameTo);
    virtual void internCalcPositions(SbVec3f* bbc,double,double,SbVec3f*,SbVec3f*,SbVec3f*,double,SbVec3f*,SbVec3f&,SbVec3f&,double&)=0;
    bool testParameters(kScriptFrameEntry*);
    void setParameters(int,int,kScriptFrameEntry*,double,SbVec3f*,SbVec3f*);
    void writeCalculatedCamPathValues(std::vector<std::string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter, int commandID);
    void writeOrientation(SbRotation orientation);
    void writePosition(SbVec3f position);
    void readParameter(kScriptFrameEntry*);
    void calcFramesPerScene(int);
    ObjMgrCommunicator* myObjMgr;
    StringField* fldAnimationViewerName;
    kScriptFrameList* myList;
    kCamera* mainCamera;
    AnimationCache* myCache;
    vector<SbVec3f>* testPositions;
    Vec3fField* fldTestSphereCenter;
    DoubleField* fldTestSphereRadius;
    Vec3fField* fldTestTargetPosition;
    BoolField* fldDrawCameraPath;
    int framesPerScene;
    ScenePosition scenePosition;
    std::string objectName,pathName;
    double maxHeight;
};
ML_END_NAMESPACE
#endif
