#include "RectangleCampath.h"
#include "kDebug.h"
#include "UMDAnimation2/AnimationExecuter.h"
ML_START_NAMESPACE
RectangleCampath::RectangleCampath(ObjMgrCommunicator* omc,StringField* favn,kScriptFrameList* m,kCamera* mc,
             AnimationCache* am,vector<SbVec3f>* tp,Vec3fField* ftsc,DoubleField* ftsr,
             Vec3fField* fttp,BoolField* fdcp,const std::string& pn):
              AbstractCampath(omc,favn,m,mc,am,tp,ftsc,ftsr,fttp,fdcp,pn)
{
  lastHeight=0.0;
}

RectangleCampath::~RectangleCampath(){}

void RectangleCampath::internCalcPositions(SbVec3f* bbc,double currentHeight,double targetHeight,SbVec3f* currentPosition,SbVec3f* currentLookAt,SbVec3f* targetLookAt,double prozdist,SbVec3f* vectorToTarget,SbVec3f& nextPosition,SbVec3f& nextLookAt,double& nextHeight){
  if(prozdist<0.25){
    nextPosition=*currentPosition;
    nextLookAt=*currentLookAt;
    //beschleunigter Steigflug
    nextHeight=currentHeight+maxHeight*pow(prozdist*4.0,2);
    //nextHeight/=(1.0-prozdist);
    lastHeight=nextHeight;
  }else if(prozdist<0.75){
    double aktpos=2.0*(prozdist-0.25);
    if(aktpos<0.5){
      //Beschleunigung
      nextPosition=*currentPosition+(pow(aktpos*2.0,2)/2.0)*(*vectorToTarget);
    }else{
      //Abbremsen
      nextPosition=*currentPosition+(1.0-(pow((1.0-aktpos)*2.0,2)/2.0))*(*vectorToTarget);
    }
    SbVec3f direction=nextPosition-(*bbc);
    nextPosition+=(fldTestSphereRadius->getDoubleValue()/direction.length()-1.0)*direction;
    nextLookAt = *currentLookAt - (*currentLookAt - *targetLookAt) * ((prozdist-0.25)*2.0);
    nextHeight=lastHeight+aktpos*(targetHeight-currentHeight);
  }else{
    nextPosition=*currentPosition + (*vectorToTarget);
    nextLookAt=*targetLookAt;
    //abgebremster Sinkflug
    nextHeight=targetHeight+maxHeight*pow((1.0-prozdist)*4.0,2);
  }
}
ML_END_NAMESPACE
