//----------------------------------------------------------------------------------
//! ML ObjMgrClient class AnimationExecuter to execute single animations.
/*!
// \file    AnimationExecuter.h
// \author  Konrad Mühler
// \date    2006-11-02
//
//
*/
//----------------------------------------------------------------------------------


#ifndef __AnimationExecuter_H
#define __AnimationExecuter_H

// Local includes
#include "AnimationSystem.h"

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include "XVEnterScope.h"
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include "XVLeaveScope.h"

#include <ObjMgr/mlObjMgrClient.h>

#include "AnimationParser/AnimationParser.h"
#include "METK.h"
#include "kBasics.h"
#include "kCamera.h"
#include "HashTable.h"
#include "mlMETKMsgSender.h"
#include "ObjMgrCommunicator.h"
#include "kDebug.h"
#include "AnimationCache.h"

//Campath-Include
#include "UMDCampath/LinearCampath.h"
#include "UMDCampath/GoogleCampath.h"
#include "UMDCampath/GoogleRectangleCampath.h"
#include "UMDCampath/RectangleCampath.h"
#include "UMDCampath/HeightCampath.h"

ML_START_NAMESPACE

//! ML ObjMgrClient class AnimationExecuter to execute single animations.
class ANIMATION_EXPORT AnimationExecuter : public ObjMgrClient
{
public:
  AnimationExecuter();
  ~AnimationExecuter();

  virtual void handleNotification (Field *field);
  virtual void handleObjMgrNotification();

  void  setObjMgr(ObjMgrCommunicator* value);
  void  setAnimationCache(AnimationCache* value);
  void  setAnimationParser(AnimationParser* value);
  void  setName(string value) { animationExecuterName=value; }
  string  getName() { return animationExecuterName; }
  void  setParent(void* value) { parent = value; }

  kCamera mainCamera; //!< Animation camera

  static const string AS_PLAY;
  static const string AS_PAUSE;
  static const string AS_STOP;

  void  runAnimation();
  void  pauseAnimation();
  void  continueAnimation();
  void  stopAnimation();

  void  ResetAll(bool refreshMeasuresCache,bool createNewParser=false);
  string  parseScript(stringstream& HLStream, const char* LLFilename, const char* CommandXMLFilename, const bool ObjectXML1Auto, const char* ObjectXMLFilename1, const char* ObjectXMLFilename2);


  //! Functions exported in ScriptCommands
  //@{
    //cmdCam.cpp
    static double getRotationAngleByType(const double targetValue, const string& interpoltype, const int& currentFrame, const int& frameCount);
    void getAngleByLastValue(const string CommandLine, const string interpoltype, const int currentFrame, const int frameCount, double& angle);
    static double Bezier(const double t);
  //@}
  bool gotClusteredCamPos,wroteClusteredCamPos,waitingFor_clusteredCamPos;
  bool gotCamPath,waitingFor_getCamPath;

private:

  typedef ObjMgrClient inherited;
  ML_CLASS_HEADER(AnimationExecuter)

  ObjMgrCommunicator* myObjMgr;
  AnimationCache* myCache;

  //! \name Module fields
  //@{
    StringField* fldAnimationViewerName;
    BoolField*   fldClearMeasureCache;
    Vec3fField* fldTestSphereCenter;
    DoubleField* fldTestSphereRadius;
    Vec3fField* fldTestCrossPoint;
    Vec3fField* fldTestCamPosition;
    Vec3fField* fldTestTargetPosition;
    DoubleField* fldTestSphereAngle;
    BoolField* fldDrawCameraPath;
    StringField* fldPlayStatus;
    FloatField* fldAnimationSpeed;
    FloatField* fldCampathVPSize;
    BoolField* fldWriteLowLevelScript;
    IntField* fldFramerate;

    NotifyField* fldOffScreenReady;
    StringField* fldOffScreenFilename;
    StringField* fldOffScreenFolder;
    NotifyField* fldOffScreenRender;
    NotifyField* fldOffScreenInit;
    NotifyField* fldOffScreenFinishAVI;
    BoolField* fldUseOffscreenRenderer;

  //@}

  bool waitingFor_getAllMeasures;
  bool waitingFor_getCamPos;
  bool waitingFor_execFading;
  bool gotCamPos;
  std::string clusteredResult;
  bool gotFading;
  int returnFrame;
  size_t returnEntry;
  string animationExecuterName; //! < name to identify AnimationExecuter out of many
  void* parent;         //! < pointer to parent to communicate the finish state

  void parseObjectCommands();
  void findObjectIDs();
  void setViewerProtocol();

  //! \name Facilities to get and wait for all measures
  //@{
    METKMsgSender* _modMsgSenderGetCamPos;
    METKMsgSender* _modMsgSenderGetCamPath;
    METKMsgSender* _modMsgSenderClusteredCamPos;
    BoolField* _msgGetCamPos_done;     //!< Listener to receive "done"-Field of _modMsgSenderGetCamPos
    BoolField* _msgGetCamPath_done;    //!< Listener to receive "done"-Field of _modMsgSenderGetCamPath
    BoolField* _msgClusteredCamPos_done;     //!< Listener to receive "done"-Field of _modMsgSenderGetClusteredCamPos
    FieldSensor* _fs_cache_getAllMeasures; //out->in(SoFile)
    static void fieldChanged_cache_getAllMeasures(void *usrData, void *sensor);
  //@}

  METKMsgSender* _modMsgSenderExecFading;
  BoolField* _msgExecFading_done;    //!< Listener to receive "done"-Field of _modMsgSenderExecFading

  //! \name Animation processing
  //@{
    int currentFrame;
    double lastFrameTime;
    bool isWaitingForRenderingReady;    //!< Process waits for renderer to be ready
    bool globalCreateVideo;         //!< Create video at the end
    void startAnimation();

    bool setFrame(int frameNr, size_t FrameEntryListIter = 0);
  //@}


  //! \name Realtime Animation
  //@{
    SoTimerSensor* realtimeTimer;     //!< Trigger for realtime animations
    double globalStartTime, globalEndTime;  //!< Time for realtime to calculate average framerate
    double _pauseAtTimeDiff;
    int globalDurchlaufCounter;       //!< Framecount for realtime to calculate average framerate
    double getCurrentTime();
    static void realtimeTimerEvent(void* data, SoDataSensor* a);
    void setRealtimeTimerAction();
  //@}

  //Zum Entkoppeln der Frames bei Video-Erzeugung
  SoTimerSensor* videoFrameTimer;
  static void videoFrameTimerEvent(void* data, SoDataSensor* a);

  //! \name Parsing
  //@{
    AnimationParser* myAnimationParser;   //!< Parser to parse script files
    kScriptFrameList myList;        //!< List of all frame to process
  //@}

  //! \name Campath
  //@{
    AbstractCampath* myCamPath;         //!< Camppathcalculator
  //@}


  //! \name Camera Movemtent Visualization
  //! Module fields to get some camera movement position for visualization
  //@{
    vector<SbVec3f> testPositions;
    SbVec3f testBBCenter;
    float testTargetHeight;
  //@}


  //! Structure for a single point in transfer function.
  struct TFPoint
  {
    float voxel;
    float opacity;
    vec3  color;
  };

  //! Functions exported in ScriptCommands
  //@{
    //cmdClip.cpp
    void commandClip(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter);

    //cmdVolRend.cpp
    void commandVolRend(/*vector<string>* tempEntryParams,*/ kScriptFrameEntry* currentEntry, int frameNr/*, size_t FrameEntryListIter*/);
    void writeCalculatedTransferFunctions(kScriptFrameEntry* currentEntry);
    void preCalcTransferFunctions(kScriptFrameEntry* frameEntry, int globalFrameNr);

    //cmdCam.cpp
    void commandRotateCam(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandRotate(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandCamFocalDistance(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandCamHeight(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandCamPosition(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandCamOrientation(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandLookAt(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandViewpointMove(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter);
    void commandCampathVP(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter);
    void commandViewpointClustering(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter);
    void commandMoveCoord(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandMove(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter);
    void commandShowClusteringPath(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter);
    void writeOrientation(SbRotation orientation);
    void writePosition(SbVec3f position);
    void oldMove(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry);
    void writeCalculatedViewpointValues(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter, int commandID);
    void writeCalculatedViewpathValues(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter, int commandID);
    void preCalcMovePositions(kScriptFrameEntry* frameEntry, int globalFrameNr, int commandID);
    void preCalcPathPositions(kScriptFrameEntry* frameEntry, int globalFrameNr, int commandID);
    void preCalcPathPositionParts(kScriptFrameEntry* frameEntry, int globalFrameNr, int frameFrom, int frameTo, SbVec3f targetPosition, SbVec3f targetLookAt,double targetHeight,SbVec3f &currentPosition,SbVec3f &currentLookAt,double &currentHeight,SbVec3f &BBCenter, double radius);
    SbVec3f getCamPosition();
    SbRotation getCamOrientation();
    double getCamHeight();
    double getCamFocalDistance();

    //cmdStandard.cpp
    void commandLayer(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandCreateVideo(vector<string>* tempEntryParams/*, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter*/);
    void commandBackground(vector<string>* tempEntryParams/*, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter*/);
    void commandColor(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandSilhouetteColor(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void commandSetType(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/);
    void setDoubleValueInterpol(const string objID, const string layerID, const string infoID, const string targetValue, const int curFrame, const int frameCount);
    void setIntegerValueInterpol(const string objID, const string layerID, const string infoID, const string targetValue, const int curFrame, const int frameCount);
    void setV3ValueInterpol(const string objID, const string layerID, const string infoID, const string targetValue, const int curFrame, const int frameCount);
    void interpolColor(string objID, string layerID, string infoID, string param255, kScriptFrameEntry* currentEntry);

    //cmdFading.cpp
    void commandFading(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter);
  //@}
};


ML_END_NAMESPACE


#endif // __AnimationExecuter_H