//! Implements the ML module class CSODistance.
//! \file CSODistance.h

#ifndef __CSODistance_H
#define __CSODistance_H

#include "../MLCSOCommunityModulesDefs.h"

#include "../MinimalDistancePointClouds/MinimalDistancePointClouds.h"
#include <mlXMarkerList.h>
#include <mlDiagramData.h> 

ML_START_NAMESPACE


//////////////////////////////////////////////////////////////////////////

//! The CSODistance calculates the smallest distance between two CSOs
class MLCSOCOMMUNITYMODULES_EXPORT CSODistance : public BaseOp
{

public:

  //! Standard constructor.
  CSODistance();

  enum {FIRST2=0, INPLANE, INDEX, LASTMODE};

protected:

  //! Standard destructor.
  virtual ~CSODistance();
  //! Initialize module after loading
  virtual void activateAttachments();
  //! Called when input changes.
  virtual void handleNotification(Field *field);


private:


  /* FIELDS */

  //! CSO input field.
  BaseField* _input0CSOFld;

  //! CSO input field.
  BaseField* _input1CSOFld;

  //! Mode
  EnumField* _modeFld;

  //! Shall the module update on every event?
  BoolField* _listenToRepaintNotificationsFld;
  //! Shall the module listen to finishing events?
  BoolField* _listenToFinishingNotificationsFld;
  //! Shall the module listen to selection change notifications?
  BoolField* _listenToSelectionChangedNotificationsFld;

  //! Only calculate distances for CSOs in the same plane.
  DoubleField* _tolleranceFld;

  //! Field for displaying the minimum distance.
  FloatField* _minimumDistanceFld;
  //! Field for displaying the position of the first point
  Vec3fField* _minimumDistancePoint1Fld;
  //! Field for displaying the position of the second point
  Vec3fField* _minimumDistancePoint2Fld;
  //! Field for holding the distances per cso
  StringField* _distancesFld;

  //! Apply field
  NotifyField* _applyFld;
  //! Auto apply field
  BoolField* _autoApplyFld;

  //! Status field
  StringField* _statusFld;

  //! XMarkerList output field
  BaseField  *_outputXMarkerListFld;

  //! CurveList output field;
  BaseField* _outputCurveListFld;

  //! Output statistic
  EnumField* _curveStatistic;

  enum STATISTICS {MIN,MAX,MEAN,STDEV,LASTSTATISTIC};
  /* MEMBER VARIABLES */

  //! A pointer to the input CSOList.
  CSOList* _csoList0;

  //! A pointer to the input CSOList.
  CSOList* _csoList1;

  //! Locking variable...
  bool _isInNotificationCB;

  //! Output XMarkerList
  XMarkerList *_outputXMarkerList;

  //! output CurveList
  CurveList* _outputCurveList;

  std::string _tableHeader;

  /* METHODS */

  //! Reads out information and displays them.
  void _process();

  //! Can be notified from anywhere in the network.
  static void _csoListNotificationCB(void* userData, int notificationFlag);

  CSO* _findMatchingCSO(int csoIndex);

  void _getDistances( std::vector<Vector3> &set1, 
                      std::vector<Vector3> &set2,
                      double               &minDistance, 
                      double               &maxDistance, 
                      double               &averageDistance, 
                      double               &stdDevDistance,
                      Vector3              &minPoint1,
                      Vector3              &minPoint2,
                      Vector3              &maxPoint1,
                      Vector3              &maxPoint2);

  ML_BASEOP_CLASS_HEADER(CSODistance);
};

//////////////////////////////////////////////////////////////////////////


ML_END_NAMESPACE


#endif // __CSODistance_H
