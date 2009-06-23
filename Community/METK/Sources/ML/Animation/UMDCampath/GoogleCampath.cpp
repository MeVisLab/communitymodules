#include "GoogleCampath.h"
#include "kDebug.h"
#include "UMDAnimation2/AnimationExecuter.h"
ML_START_NAMESPACE
GoogleCampath::GoogleCampath(ObjMgrCommunicator* omc,StringField* favn,kScriptFrameList* m,kCamera* mc,
             AnimationCache* am,vector<SbVec3f>* tp,Vec3fField* ftsc,DoubleField* ftsr,
             Vec3fField* fttp,BoolField* fdcp,const std::string& pn):
              AbstractCampath(omc,favn,m,mc,am,tp,ftsc,ftsr,fttp,fdcp,pn)
{}

GoogleCampath::~GoogleCampath(){}

void GoogleCampath::internCalcPositions(SbVec3f* bbc,double currentHeight,double targetHeight,SbVec3f* currentPosition,SbVec3f* currentLookAt,SbVec3f* targetLookAt,double prozdist,SbVec3f* vectorToTarget,SbVec3f& nextPosition,SbVec3f& nextLookAt,double& nextHeight){
  nextPosition = *currentPosition + prozdist*(*vectorToTarget);
  SbVec3f direction=nextPosition-(*bbc);
  nextPosition+=(fldTestSphereRadius->getDoubleValue()/direction.length()-1.0)*direction;
  nextLookAt = *currentLookAt - (*currentLookAt - *targetLookAt) * prozdist;
  double powFak=2.0*(prozdist-0.5);
  nextHeight=(1.0-prozdist)*currentHeight+prozdist*targetHeight+maxHeight;
  if(prozdist>=0.5){
    nextHeight=prozdist*targetHeight+(1.0-prozdist)*currentHeight+maxHeight;
  }
  nextHeight-=pow(powFak,2)*maxHeight;
}
ML_END_NAMESPACE
