
//! The ML ObjMgrClient module METKKeyStates.
//! \file METKKeyStates.h

#ifndef __METKKeystates_H
#define __METKKeystates_H


// Local includes
#include "ViewpointSystem.h"

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>
#include <ObjMgr/mlObjMgr.h>
#include <ObjDump/mlObjDump.h>
#include <ObjLoader/mlObjLoader.h>
#include <ObjConstrainedIterator/mlObjConstrainedIterator.h>
#include <ObjInfo/mlObjInfo.h>
#include <inttypes.h>

#include "kBasics.h"
#include "HashTable.h"
#include "kCamera.h"
#include "kDebug.h"
#include "METK.h"

#include "vpBasic/vpCalcVis.h"
#include "vpBasic/vpField.h"

#include "METKMsgManager.h"
#include "ObjMgrCommunicator.h"

#include <Inventor/sensors/SoTimerSensor.h>

#include "UMDVisDataViewer/SoVisDataViewer.h"

//class SoSeparator;
class SoVisDataViewer;
class SoSwitch;
ML_START_NAMESPACE

//! The ML ObjMgrClient module METKKeyStates.
class VIEWPOINT_EXPORT METKKeystates : public ObjMgrClient
{
public:

  //! Constructor.
  METKKeystates();
  ~METKKeystates (void);

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
  NotifyField* _calc;
  NotifyField* _init;
  IntField* _showField;
  enum sphereModeType { SM_GRID = 1, SM_FULL = 2 };
  static const char *sphereModeStrings[2];
  EnumField* _sphereMode;
  StringField* _dataPath;
  StringField* _currentStructure;
  StringField* _debug;
  StringField* _viewerName;
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
  DoubleField* _wKeystateVPDirection;
  Vec3fField*  _prefRegionVector;
  DoubleField* _prefRegionRange;
  EnumField*   _prefRegionType;
  enum prefRegionType { PR_VECTOR = 0, PR_POINT = 1 };
  static const char *prefRegionTypeStrings[2];
  BoolField*  _restrictToRegion;
  Vec3fField* _result;
  Vec4fField* _orient;
  StringField* _messageData;
  StringField* _message;
  BoolField* _setViewerCamAtTheEnd;
  NotifyField* _writeCamToObjMgr;
  EnumField* _debugState;
  StringField* _datInfo;
  StringField* _similarityVector;
  NotifyField* _test;
  StringField* _keystateFilename;
  NotifyField* _loadFile;
  NotifyField* _saveFile;
  NotifyField* _createNewState;
  StringField* _currentKeystate;
  NotifyField* _applyCurrentKeystate;
  StringField* _animationScript;
  NotifyField* _clearKeystates;
  enum referenceGroupType { RG_STRUCTURE = 0, RG_STRUCTUREGROUP = 1, RG_REALNAME = 2 };
  static const char *referenceGroupStrings[3];
  EnumField* _referenceGroup;
  IntField* _animationStepLength;
  IntField* _animationStepPause;
  NotifyField* _createAnimationFromKeystates;
  BoolField* _animationAsVideo;



  //! Access field to internal ObjMgr
  BaseField  *internalObjMgrOutFld;

  kCamera Cam;
  ObjMgrCommunicator _myMETK;
  ObjMgr _objMgrKeystate;
  ObjMgrCommunicator _myKeystateCommunicator;
  ObjDump _objDumpKeystate;
  ObjLoader _objLoaderKeystate;
  ObjConstrainedIterator _objIterMETK;
  ObjInfo _objInfoKeystates;
  ObjConstrainedIterator _objIterKeystates;

  string path;
  SoVisDataViewer* m_soViewer;
  CvpCalcVis m_calcVis;

  vector<string*> m_vStructures;

  METKMsgReceiver oReceiver;
  SoTimerSensor* timerSensor; //for doneFld touching with a little delay
  static void timerEvent(void* data, SoDataSensor* a);
  void updateObjectMgr();
  void writeCamToObjMgr();
  void enableVisibleObjects();
  const float getImportance(const std::string* name);
  void init();
  string generateKeystate();
  void generateViewpointData(string keystateID);
  bool generateViewpointFromKeystate(string keystateID,vec3& position);
  void generateScreenshot(string keystateID);
  void generateAppearanceData(string keystateID);
  void setKeystatePath();
  string applyKeystate(string keystateID, string timecode, bool singleKeystate);
  void generateAnimation();

  //! Implements interface for the runtime type system of the ML.
  ML_CLASS_HEADER(METKKeystates)
};


ML_END_NAMESPACE

#endif // __METKKeystates_H