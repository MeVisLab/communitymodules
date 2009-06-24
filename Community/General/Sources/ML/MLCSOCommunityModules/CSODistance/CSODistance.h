
#ifndef __CSODistance_H
#define __CSODistance_H

#include "../MLCSOCommunityModulesDefs.h"

#include "../MinimalDistancePointClouds/MinimalDistancePointClouds.h"
#include <mlXMarkerList.h>

ML_START_NAMESPACE


//////////////////////////////////////////////////////////////////////////

//! The CSODistance calculates the smallest distance between two CSOs
class MLCSOCOMMUNITYMODULES_EXPORT CSODistance : public BaseOp
{ 

public:

  //! Standard constructor.
  CSODistance();

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
  BaseField* _inputCSOFld;

  //! Shall the module update on every event?
  BoolField* _listenToRepaintNotificationsFld;
  //! Shall the module listen to finishing events?
  BoolField* _listenToFinishingNotificationsFld;
  //! Shall the module listen to selection change notifications?
  BoolField* _listenToSelectionChangedNotificationsFld;

  //! Field for displaying the minimum distance.
  FloatField* _minimumDistanceFld;
  //! Field for displaying the position of the first point
  Vec3fField* _minimumDistancePoint1Fld;
  //! Field for displaying the position of the second point
  Vec3fField* _minimumDistancePoint2Fld;

  //! Apply field
  NotifyField* _applyFld;
  //! Auto apply field
  BoolField* _autoApplyFld;

  //! XMarkerList output field
	BaseField  *_outputXMarkerListFld;

  /* MEMBER VARIABLES */

  //! A pointer to the input CSOList.
  CSOList* _csoList;

  //! Locking variable...
  bool _isInNotificationCB;

  //! Output XMarkerList
	XMarkerList *_outputXMarkerList;


  /* METHODS */

  //! Reads out information and displays them.
  void _process();

  //! Can be notified from anywhere in the network.
  static void _csoListNotificationCB(void* userData, int notificationFlag);

  ML_BASEOP_CLASS_HEADER(CSODistance);
};

//////////////////////////////////////////////////////////////////////////


ML_END_NAMESPACE


#endif // __CSODistance_H
