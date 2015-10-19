
//! The ML ObjMgrClient module METKCalcCamPos.
//! \file METKCalcCamPos.h

#ifndef __METKCalcCamPos_H
#define __METKCalcCamPos_H


// Local includes
#include "ViewpointSystem.h"

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>
#include <inttypes.h>

#include "kBasics.h"
#include "kCamera.h"
#include "kDebug.h"
#include "METK.h"

#include "vpBasic/vpCalcVis.h"
#include "vpBasic/vpField.h"

#include "METKMsgManager.h"
#include "ObjMgrCommunicator.h"

#include <Inventor/sensors/SoTimerSensor.h>

//class SoSeparator;
class SoVisDataViewer;
class SoVisDataViewer2D;
class SoSwitch;
ML_START_NAMESPACE

//! The ML ObjMgrClient module METKCalcCamPos.
class VIEWPOINT_EXPORT METKCalcCamPos : public ObjMgrClient
{
public:

  //! Constructor.
  METKCalcCamPos();
  ~METKCalcCamPos (void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);
  virtual void handleObjMgrNotification();
  virtual void activateAttachments();
  void setCamPosition(const int stackOrFieldNr, const bool isStackNr);

private:

  typedef ObjMgrClient inherited;

  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------
  //@}

  SoNodeField* _outScene;
  SoNodeField* _outScene2;
  NotifyField* _calc;
  NotifyField* _calcMultiple;
  NotifyField* _init;
  IntField* _showField;
  enum sphereModeType { SM_GRID = 1, SM_FULL = 2 };
  static const char *sphereModeStrings[2];
  EnumField* _sphereMode;
  StringField* _dataPath;
  StringField* _currentStructure;
  StringField* _multipleStructures;
  StringField* _debug;
  StringField* _viewerName;
  /*DoubleField* _camX;
  DoubleField* _camY;
  DoubleField* _camZ;*/
  Vec3fField* _currentCam;
  DoubleField* _camRange;
  DoubleField* _visSta;
  DoubleField* _impSta;
  IntField* _inspect;
  DoubleField* _wVis;
  DoubleField* _wImp;
  DoubleField* _wNum;
  DoubleField* _wEnt;
  DoubleField* _wDis;
  DoubleField* _wCam;
  DoubleField* _wReg;
  DoubleField* _wVisSta;
  DoubleField* _wImpSta;
  DoubleField* _wSilhouette;
  DoubleField* _wImageSpaceCenter;
  Vec3fField*  _prefRegionVector;
  DoubleField* _prefRegionRange;
  EnumField*   _prefRegionType;
  enum prefRegionType { PR_VECTOR = 0, PR_POINT = 1 };
  static const char *prefRegionTypeStrings[2];
  BoolField*  _restrictToRegion;
  /*DoubleField* _minX;
  DoubleField* _minY;
  DoubleField* _minZ;*/
  Vec3fField* _minDistVec;
  DoubleField* _minRange;
  NotifyField* _calcMin;
  /*DoubleField* _resX;
  DoubleField* _resY;
  DoubleField* _resZ;*/
  Vec3fField* _result;
  Vec4fField* _orient;
  StringField* _messageData;
  StringField* _message;
  BoolField* _setViewerCamAtTheEnd;
  NotifyField* _writeCamToObjMgr;
  EnumField* _debugState;
  StringField* _datInfo;

   kCamera *Cam;
   ObjMgrCommunicator* myObjMgr;

   SoSwitch* _Switch;
   string path;
   string* pathIds;
   SoVisDataViewer* m_soViewer;
   SoVisDataViewer2D* m_soViewer2D;
   CvpCalcVis m_calcVis;

   vector<string*> m_vStructures;

   METKMsgReceiver* oReceiver;
   SoTimerSensor* timerSensor; //for doneFld touching with a little delay
   static void timerEvent(void* data, SoDataSensor* a);
   void updateObjectMgr();
   void writeCamToObjMgr();
   void enableVisibleObjects();
   const float getImportance(const std::string* name);
   void init();
   void calcMultiple(bool setCamera=true);

   //! Implements interface for the runtime type system of the ML.
   ML_CLASS_HEADER(METKCalcCamPos)
};


ML_END_NAMESPACE

#endif // __METKCalcCamPos_H