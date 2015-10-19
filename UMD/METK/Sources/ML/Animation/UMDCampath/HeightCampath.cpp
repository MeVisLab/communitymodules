#include "HeightCampath.h"
#include "kDebug.h"
#include "UMDAnimation2/AnimationExecuter.h"
ML_START_NAMESPACE
HeightCampath::HeightCampath(ObjMgrCommunicator* omc,StringField* favn,kScriptFrameList* m,kCamera* mc,
             AnimationCache* am,vector<SbVec3f>* tp,Vec3fField* ftsc,DoubleField* ftsr,
             Vec3fField* fttp,BoolField* fdcp,const std::string& pn):
              AbstractCampath(omc,favn,m,mc,am,tp,ftsc,ftsr,fttp,fdcp,pn)
{}

HeightCampath::~HeightCampath(){}

void HeightCampath::internCalcPositions(SbVec3f* bbc,double currentHeight,double targetHeight,SbVec3f* currentPosition,SbVec3f* currentLookAt,SbVec3f* targetLookAt,double prozdist,SbVec3f* vectorToTarget,SbVec3f& nextPosition,SbVec3f& nextLookAt,double& nextHeight){
  if(prozdist<0.5){
    //Beschleunigung
    nextPosition=*currentPosition+(pow(prozdist*2.0,2)/2.0)*(*vectorToTarget);
  }else{
    //Abbremsen
    nextPosition=*currentPosition+(1.0-(pow((1.0-prozdist)*2.0,2)/2.0))*(*vectorToTarget);
  }
  SbVec3f direction=nextPosition-(*bbc);
  nextPosition+=(fldTestSphereRadius->getDoubleValue()/direction.length()-1.0)*direction;
  nextLookAt = *currentLookAt - (*currentLookAt - *targetLookAt) * prozdist;
  nextHeight = currentHeight - (currentHeight - targetHeight) * prozdist;
  //std::cout << "NEXTHEIGHT::" << nextPosition[0] << ";" << nextPosition[1] << ";" << nextPosition[2] << "::" << nextHeight << "::" << len << std::endl;
}
ML_END_NAMESPACE
