
#include "CSODistance.h"

ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

ML_BASEOP_CLASS_SOURCE(CSODistance, BaseOp);

//////////////////////////////////////////////////////////////////////////

CSODistance::CSODistance() : BaseOp(0, 0)
{
  ML_TRACE_IN("CSODistance::CSODistance(int numInImages, int numOutImages) : BaseOp(numInImages, numOutImages)");

  handleNotificationOff();

  _csoList = NULL;

  FieldContainer* fieldC = getFieldContainer();
  ML_CHECK(fieldC);

  (_inputCSOFld = fieldC->addBase("inCSOList"))->setBaseValue(NULL);

  //////////////////////////////////////////////////////////////////////////

  (_minimumDistanceFld = fieldC->addFloat("minimumDistance"))->setFloatValue(0);
  (_minimumDistancePoint1Fld = fieldC->addVec3f("minimumDistancePoint1"))->setVec3fValue(vec3(0.0,0.0,0.0));
  (_minimumDistancePoint2Fld = fieldC->addVec3f("minimumDistancePoint2"))->setVec3fValue(vec3(0.0,0.0,0.0));

  //////////////////////////////////////////////////////////////////////////

  _applyFld = fieldC->addNotify("apply");
  (_autoApplyFld = fieldC->addBool("autoApply"))->setBoolValue(true);

  //////////////////////////////////////////////////////////////////////////

  (_listenToRepaintNotificationsFld          = fieldC->addBool("listenToRepaintNotifications"))
    ->setBoolValue(false);
  (_listenToFinishingNotificationsFld        = fieldC->addBool("listenToFinishingNotifications"))
    ->setBoolValue(true);
  (_listenToSelectionChangedNotificationsFld = fieldC->addBool("listenToSelectionChangedNotifications"))
    ->setBoolValue(false);

  _isInNotificationCB = false; 	
  
  ML_CHECK_NEW(_outputXMarkerList,XMarkerList());

	(_outputXMarkerListFld = getFieldContainer()->addBase("outputXMarkerList"))->setBaseValue(NULL);


  handleNotificationOn();
}

//////////////////////////////////////////////////////////////////////////

CSODistance::~CSODistance()
{
  ML_TRACE_IN("CSODistance::~CSODistance()");

  if (_csoList != NULL){
    _csoList->removeNotificationObserver(_csoListNotificationCB, this);
  }

  ML_DELETE(_outputXMarkerList);
}

//////////////////////////////////////////////////////////////////////////

void CSODistance::activateAttachments()
{
  ML_TRACE_IN("void CSODistance::activateAttachments()");

  _minimumDistanceFld->setFloatValue(0);
  _minimumDistancePoint1Fld->setVec3fValue(vec3(0.0,0.0,0.0));
  _minimumDistancePoint2Fld->setVec3fValue(vec3(0.0,0.0,0.0));

  BaseOp::activateAttachments();
}

//////////////////////////////////////////////////////////////////////////

void CSODistance::handleNotification(Field* field)
{
  ML_TRACE_IN("void CSODistance::handleNotification(Field* field)");

  if (field == _inputCSOFld){

    if (!_isInNotificationCB) { CSOList::removeNotificationObserverFromAllCSOLists(_csoListNotificationCB, this);}

    if (BASE_IS_A((static_cast<Base*>(_inputCSOFld->getBaseValue())), CSOList)){

      _csoList = static_cast<CSOList*>(_inputCSOFld->getBaseValue());

      if (_csoList != NULL){

        if (!_isInNotificationCB) { _csoList->addNotificationObserver(_csoListNotificationCB, this); }
      }
    } else {

      _csoList = NULL;
    }

    if (_autoApplyFld->getBoolValue()) { _process(); }
  } else
  if (field == _applyFld) {
    _process();
  }


  BaseOp::handleNotification(field);
}

//////////////////////////////////////////////////////////////////////////

void CSODistance::_process()
{   
  ML_TRACE_IN("void CSODistance::_process()");

  _outputXMarkerList->clearList();

  if (_csoList != NULL) {

    if ((_csoList->numCSO() >= 2) && (_csoList->getCSOAt(0)->getIsFinished()) && (_csoList->getCSOAt(1)->getIsFinished())) {
      MinimalDistancePointClouds* pointSetsMinDist = NULL;
      ML_CHECK_NEW(pointSetsMinDist, MinimalDistancePointClouds);

      std::vector<vec3>pointSet1;
      std::vector<vec3>pointSet2;
      
      _csoList->getCSOAt(0)->fillPathPointCoordinatesFlattened(pointSet1);
      _csoList->getCSOAt(1)->fillPathPointCoordinatesFlattened(pointSet2);

      pointSetsMinDist->setPointSets(pointSet1, pointSet2);
      pointSetsMinDist->setNumEntries(200);

      vec3 nPoint1, nPoint2;
      pointSetsMinDist->computeDistance(nPoint1,nPoint2);

      _minimumDistancePoint1Fld->setVec3fValue(nPoint1);
      _minimumDistancePoint2Fld->setVec3fValue(nPoint2);

      double distance = sqrt((nPoint1[0]-nPoint2[0])*(nPoint1[0]-nPoint2[0]) + 
                             (nPoint1[1]-nPoint2[1])*(nPoint1[1]-nPoint2[1]) + 
                             (nPoint1[2]-nPoint2[2])*(nPoint1[2]-nPoint2[2]));

      _minimumDistanceFld->setFloatValue(distance);

      _outputXMarkerList->appendItem(XMarker(vec6(nPoint1[0],nPoint1[1],nPoint1[2],0.5f,0.0f,0.0f),
                                             vec3(nPoint2[0]-nPoint1[0],nPoint2[1]-nPoint1[1],0.0f),
                                             0,""));
      
      ML_DELETE(pointSetsMinDist);

    } else {
      _minimumDistancePoint1Fld->setVec3fValue(vec3(0.0,0.0,0.0));
      _minimumDistancePoint2Fld->setVec3fValue(vec3(0.0,0.0,0.0));
      _minimumDistanceFld->setFloatValue(0.0f);
    }
  } else {
    _minimumDistancePoint1Fld->setVec3fValue(vec3(0.0,0.0,0.0));
    _minimumDistancePoint2Fld->setVec3fValue(vec3(0.0,0.0,0.0));
    _minimumDistanceFld->setFloatValue(0.0f);
  }

  _outputXMarkerListFld->setBaseValue(_outputXMarkerList);

}

//////////////////////////////////////////////////////////////////////////

void CSODistance::_csoListNotificationCB(void* userData, int notificationFlag)
{
  ML_TRACE_IN("void CSODistance::_csoListNotificationCB()");

  CSODistance* thisp = static_cast<CSODistance*>(userData);

  thisp->_isInNotificationCB = true;
  bool autoApply = thisp->_autoApplyFld->getBoolValue();

  if ((notificationFlag & CSOList::NOTIFICATION_CSO_SELECTION) ||
      (notificationFlag & CSOList::NOTIFICATION_GROUP_SELECTION))
  {
      if (thisp->_listenToSelectionChangedNotificationsFld->getBoolValue()){
        if (autoApply) { thisp->_process(); }
      }
  }
  if ((notificationFlag & CSOList::NOTIFICATION_CSO_FINISHED) ||
      (notificationFlag & CSOList::NOTIFICATION_GROUP_FINISHED))
  {
      if (thisp->_listenToFinishingNotificationsFld->getBoolValue()){
        if (autoApply) { thisp->_process(); }
      }
  }
  if (notificationFlag & CSOList::NOTIFICATION_REPAINT){
    if (thisp->_listenToRepaintNotificationsFld->getBoolValue()){
        if (autoApply) { thisp->_process(); }
    }
  }
  if (notificationFlag & CSOList::NOTIFICATION_INTERACTION_INIT){
    // do nothing
  }

  thisp->_isInNotificationCB = false;
}

//////////////////////////////////////////////////////////////////////////


ML_END_NAMESPACE

