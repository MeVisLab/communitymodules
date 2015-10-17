//----------------------------------------------------------------------------------
//! ML ObjMgrClient class UMDAnimation2 for Konrad Muehlers Animation in MeVisLab.
/*!
// \file    mlUMDAnimation2.h
// \author  Konrad Mühler
// \date    2005-05-04
//
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlUMDAnimation2_H
#define __mlUMDAnimation2_H

// Local includes
#include "AnimationSystem.h"

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include "SomnOffscreenRenderer.h"

#include "XVEnterScope.h"
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include "XVLeaveScope.h"

#include <ObjMgr/mlObjMgrClient.h>

#include "AnimationParser/AnimationParser.h"
#include "METK.h"
#include "kBasics.h"
#include "kCamera.h"
#include "HashTable.h"
#include "mlMETKMsgSender.h"
#include "kDebug.h"
#include "AnimationExecuter.h"

ML_START_NAMESPACE



//! ML ObjMgrClient class UMDAnimation2 for Konrad Muehlers Animation in MeVisLab.
class ANIMATION_EXPORT UMDAnimation2 : public ObjMgrClient
{
public:

  UMDAnimation2 (void);

  virtual void handleNotification (Field *field);
  virtual void handleObjMgrNotification();
  virtual void activateAttachments();

  void animationFinished(string name);

private:

  typedef ObjMgrClient inherited;

  ML_CLASS_HEADER(UMDAnimation2)


  //! \name Module fields
  //@{
    StringField* fldModulePath;
    NotifyField* fldScriptExecute;
    EnumField*   fldPathTypeScriptPath;
    StringField* fldScriptPath;
    StringField* fldScriptFile;
    EnumField*   fldPathTypeObjectNamesPath1;
    StringField* fldObjectNamesPath1;
    StringField* fldObjectNamesFile1;
    BoolField*   fldUseObjectNamesFile2;
    EnumField*   fldPathTypeObjectNamesPath2;
    StringField* fldObjectNamesPath2;
    StringField* fldObjectNamesFile2;
    EnumField*   fldPathTypeHighLevelCommandsPath;
    StringField* fldHighLevelCommandsPath;
    StringField* fldHighLevelCommandsFile;
    StringField* fldCommandLine;
    NotifyField* fldCommandLineExecute;
    StringField* fldScriptArea;
    NotifyField* fldScriptAreaExecute;
    NotifyField* fldReset;
    StringField* fldAnimationViewerName;
    EnumField* fldDebugState;
    BoolField*   fldClearMeasureCache;
    Vec3fField* fldTestSphereCenter;
    DoubleField* fldTestSphereRadius;
    Vec3fField* fldTestCrossPoint;
    Vec3fField* fldTestCamPosition;
    Vec3fField* fldTestTargetPosition;
    DoubleField* fldTestSphereAngle;
    BoolField* fldDrawCameraPath;
    BoolField* fldEnableSelection;
    FloatField* fldAnimationSpeed;
    FloatField* fldCampathVPSize;

    NotifyField* fldPauseAnimation;
    NotifyField* fldContinueAnimation;
    NotifyField* fldStopAnimation;
    StringField* fldPlayStatus;

    BoolField* fldWriteLowLevelScript;
    IntField* fldFramerate;

    NotifyField* fldOffScreenReady;
    StringField* fldOffScreenFilename;
    StringField* fldOffScreenFolder;
    NotifyField* fldOffScreenRender;
    NotifyField* fldOffScreenInit;
    NotifyField* fldOffScreenFinishAVI;
    IntField* fldVideoXSize;
    IntField* fldVideoYSize;
    BoolField* fldSaveFrames;
    BoolField* fldSaveVideo;
    BoolField* fldUseOffscreenRenderer;



    SoNodeField* inInventor;



  //@}


  //! \name Pathtypes
  //! Enum values to select the pathtype in module interface
  //@{
    enum pathType { PT_SCRIPTPATH = 0, PT_MODULEPATH, PT_CASEPATH, PT_OTHERPATH, NUM_PATHTYPE };
    enum pathTypeObjXML1 { PTO1_SCRIPTPATH = 0, PTO1_MODULEPATH, PTO1_CASEPATH, PTO1_OTHERPATH, PTO1_AUTO, NUM_PATHTYPEO1 };
    static const char *pathTypeStrings[NUM_PATHTYPE];
    static const char *pathTypeStringsObjXML1[NUM_PATHTYPEO1];
    enum scriptPathType { SPT_MODULEPATH, SPT_CASEPATH, SPT_OTHERPATH, NUM_SCRIPTPATHTYPE};
    static const char *scriptPathTypeStrings[NUM_SCRIPTPATHTYPE];
  //@}


  static const string AEX_MAIN;
  static const string AEX_TRANSFER;

  void calcFolders(string& scriptPath, string& scriptFile, string& objectNamesPath1, string& objectNamesFile1, string& objectNamesPath2, string& objectNamesFile2, string& commandsPath, string& commandsFile);
  void ObjectSelected(string objectID);

  AnimationParser* myAnimationParser;   //!< Parser to parse script files

  AnimationExecuter* currentAnimation;
  AnimationExecuter* transferAnimation;
  ObjMgrCommunicator* myObjMgr;
  SomnOffscreenRenderer* offScreen;

  void createFieldConnectionsForAnimationExecuter(AnimationExecuter* aex, bool inToOut);


  SoFieldSensor* _fieldSensor;
  static void fieldChangedCB(void *data, SoSensor* sens);
  void fieldChanged(SoFieldSensor* sensor);

};


ML_END_NAMESPACE

#endif // __mlUMDAnimation2_H