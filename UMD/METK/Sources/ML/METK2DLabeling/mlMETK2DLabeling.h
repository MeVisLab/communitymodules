//----------------------------------------------------------------------------------
//! ML ObjMgrClient class METK2DLabeling to label MCSM in 2d.
/*!
// \file    mlMETK2DLabeling.h
// \author  Konrad Mühler
// \date    2008-04-11
//
// Module to label MCSM in 2d
*/
//----------------------------------------------------------------------------------


#ifndef __mlMETK2DLabeling_H
#define __mlMETK2DLabeling_H


//#include <SoView2DExtensionExample.h>

// Local includes
#include "METK2DLabelingSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <ObjMgr/mlObjMgrClient.h>

#include <vector>
#include <map>
#include "kBasics.h"
#include "HashTable.h"
#include "labField.h"
#include "../METK.h"
#include "ObjMgrCommunicator.h"
#include <SoDraw2DLabels/SoDraw2DLabels.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include "ObjConstrainedIterator/mlObjConstrainedIterator.h"

ML_START_NAMESPACE


//! ML ObjMgrClient class to label MCSM in 2d.
class METK2DLABELING_EXPORT METK2DLabeling : public ObjMgrClient
{
public:

  //! Constructor.
  METK2DLabeling();
  ~METK2DLabeling();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

  virtual void handleObjMgrNotification();

  //! Update internal module state after load or clone and enable notification handling again.
  virtual void activateAttachments ();

  //! Configures (in)validation handling of inputs which are not connected or up to date.
  virtual INPUT_HANDLE handleInput(int inIndex, INPUT_STATE state) const;

  // ----------------------------------------------------------
  //@{ \name Image processing methods.
  // ----------------------------------------------------------

  //! Sets properties of the output image at output outIndex.
  virtual void calcOutImageProps (int outIndex);

  //! Calculates page outSubImg of output image with index outIndex by using inSubimgs.
  //! \param outSubImg The subimage of output image outIndex calculated from inSubImgs.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImgs Array of subimage(s) of the input(s) whose extents were specified
  //!                  by calcInSubImageBox. Array size is given by getInputNum().
  virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImgs);

  //! Method template for type-specific page calculation. Called by calcOutSubImage().
  //! \param outSubImg The typed subimage of output image outIndex calculated from inSubImg.
  //! \param outIndex  The index of the output the subimage is calculated for.
  //! \param inSubImg0 Temporary subimage of input 0.
  template <typename T>
  void calcOutSubImage (TSubImg<T> *outSubImg, int outIndex
    , TSubImg<T> *inSubImg0
    );
  //@}


private:

  typedef ObjMgrClient inherited;

  //!
  IntField *_sliceFld;
  BoolField *_enableLabelingFld;
  IntField *_maxConflictIterationsFld;
  DoubleField *_transpThresholdFld;

  //!
  NotifyField *_calculateFld;
  NotifyField *_initPlacementFld;
  NotifyField *_refineFld;
  NotifyField *_refineSingleStepFld;

  IntField *_optFactorFld;

  //!
  EnumField *_debugOutputFld;
  enum debugOutput { DOUT_NONE, DOUT_VOXEL, DOUT_COMP, DOUT_DISTANCE, DOUT_ANCHORDIST, DOUT_TEXTH1, DOUT_TEXTH2, DOUT_FREESPACE, DOUT_LABELBOXES, NUM_DEBUGOUTPUT};
  static const char *debugOutputStrings[NUM_DEBUGOUTPUT];
  EnumField *_debugStateFld;
  enum debugState { DSTATE_NONE, DSTATE_ALL, NUM_DEBUGSTATEOUTPUT};
  static const char *debugStateStrings[NUM_DEBUGSTATEOUTPUT];

  IntField *_anchorPointInspectionFld;

  IntField *_borderDistanceFld;

  IntField *_minCompSizeFld; //Distance of the center point that must be reached at least
  IntField *_maxGroupDistanceFld; //Maximum anchor point distance between group components
  IntField *_encircleSizeFld;

  ColorField *_textColorFld;
  ColorField *_boxColorFld;
  DoubleField *_boxTransparencyFld;
  ColorField *_borderColorFld;
  ColorField *_lineColorFld;
  BoolField *_autoBoxColorFld;
  IntField *_innerBorderFld;

  BoolField *_useAdditionalMaskFld;

  BoolField *_sendCSOFld;
  StringField *_CSOFld;



  labField *_voxelValues;
  labField *_additionalMask;
  labField *_componentIDs;
  labField *_globalDistanceField;
  std::vector<labField*> _textFreeSpaceHorizontalField;
  labField *_tempFieldForLabelBoxes;

  Vector _sliceSizeOpt;
  Vector _sliceSizeOrg;


  SoNodeField* _outInventor;
  SoDraw2DLabels* _2dLabelsNode;

  int _optFactor;
  int _currentSlice;

  struct SINGLECOMPONENT {
    int maxInnerDistance;
    int voxelCount;
    kBasics::POINT anchorPoint;
    kBasics::POINT lineEndPoint;
    kBasics::POINT labelPoint;
    string labelText;
    kBasics::POINT textSize;
    kBasics::POINT totalSize;
    labField* freeSpaceField;
    labField* anchorDistanceField;
    int parentID;
    int groupID;
    string objectID;
    int importanceLayer; //0 for most important; 1 for less important
    bool valid;
  };

  struct LINECOMPONENT {
    kBasics::LINE line;
    int componentID;
    int lineType; //! LT_CONNECTIONLINE or LT_TEXTBB
  };

  struct BOXCOMPONENT {
    kBasics::POINT corner1;
    kBasics::POINT corner2;
    int componentID;
  };

  struct LABELCONFLICT {
    int componentID1,componentID2;
    int intersectionType; //!IT_CL_CL, IT_CL_TEXT, IT_TEXT_TEXT
  };

  static const int LT_CONNECTIONLINE;
  static const int LT_TEXTBB;
  static const int IT_CL_CL;
  static const int IT_CL_TEXT;
  static const int IT_TEXT_TEXT;
  static const int DIRTYPE_UPRIGHT;
  static const int DIRTYPE_DOWNLEFT;
  static const int DIRTYPE_UPLEFT;
  static const int DIRTYPE_DOWNRIGHT;
  static const int DIRTYPE_UP;
  static const int DIRTYPE_DOWN;
  static const int DIRTYPE_LEFT;
  static const int DIRTYPE_RIGHT;

  std::vector<SINGLECOMPONENT> _components;
  std::list<LINECOMPONENT> _labelLines;
  std::list<BOXCOMPONENT> _labelBoxes;
  std::vector<LABELCONFLICT> _labelConflicts;
//  HashTable<string> _ht_voxelValObjID;
  std::vector<string> _voxelValObjID;
  HashTable<double> _htObjIDTransp;

  void initialPlacement();
  void refineAll();
  template <typename DT>
  void calculateLabels(MLDataType ML_DT);
  int calcConnectedComp();
  void calcGlobalDistanceField();
  void calcGroups();
  void calcAnchorPointDistances();
  void calcFreeSpace();
  void calcTextPos();
  void transferLabelsToInventor();
  void disableLabelPosition(const int i);
  void debugLabelsIn2D(const int i);
  int resolveConflicts();
  void buildObjIDList();

  bool findNewPosition(int compID, kBasics::POINT searchPoint, int dirType, int minDist);
  void calcLabelLine(kBasics::POINT anchorPoint, kBasics::POINT labelPoint, kBasics::POINT labelSize, kBasics::POINT &lineEndPoint);
  void deleteLinesFromCache(int id);
  void addLabelLinesToCache(int id);
  void checkForConflicts(LINECOMPONENT tempLine);
  void checkForConflicts(BOXCOMPONENT tempBox);

  SoTimerSensor* timerSensor; //for doneFld touching with a little delay
  static void timerEvent(void* data, SoDataSensor* a);

  ObjMgrCommunicator* myObjMgr;
  ObjConstrainedIterator* myObjIterator;


  SoFieldSensor* _fieldSensor;
  static void fieldChangedCB(void *data, SoSensor* sens);
  void fieldChanged(SoFieldSensor* sensor);

  //! Implements interface for the runtime type system of the ML.
  ML_CLASS_HEADER(METK2DLabeling)
};


ML_END_NAMESPACE

#endif // __mlMETK2DLabeling_H

