
#include "CSODistance.h"

ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

ML_BASEOP_CLASS_SOURCE(CSODistance, BaseOp);

//////////////////////////////////////////////////////////////////////////

CSODistance::CSODistance() : BaseOp(0, 0)
{
  ML_TRACE_IN("CSODistance::CSODistance(int numInImages, int numOutImages) : BaseOp(numInImages, numOutImages)");

  handleNotificationOff();

  _csoList0 = NULL;
  _csoList1 = NULL;
  _tableHeader = "Id1,Id2,min,max,mean,stdDev";

  FieldContainer* fieldC = getFieldContainer();
  ML_CHECK(fieldC);

  (_input0CSOFld = fieldC->addBase("inCSOList"))->setBaseValue(NULL);
  (_input1CSOFld = fieldC->addBase("inCSOList1"))->setBaseValue(NULL);

  //////////////////////////////////////////////////////////////////////////

  char* distanceModes[LASTMODE];
  distanceModes[ FIRST2 ] = "First 2 CSOs";
  distanceModes[ INPLANE ] = "Match in plane";
  distanceModes[ INDEX ] = "Match index";
  _modeFld = fieldC->addEnum("mode",distanceModes,LASTMODE);
  _modeFld->setEnumValue( FIRST2 );

  //////////////////////////////////////////////////////////////////////////

  (_minimumDistanceFld = fieldC->addFloat("minimumDistance"))->setFloatValue(0);
  (_minimumDistancePoint1Fld = fieldC->addVec3f("minimumDistancePoint1"))->setVec3fValue(vec3(0.0,0.0,0.0));
  (_minimumDistancePoint2Fld = fieldC->addVec3f("minimumDistancePoint2"))->setVec3fValue(vec3(0.0,0.0,0.0));
  _distancesFld = addString("distances",_tableHeader );

  _AverageMinimumDistanceFld = addFloat("averageMinimumDistance", 0.0f );
  _AverageMeanDistanceFld    = addFloat("averageMeanDistance", 0.0f );
  _AverageMaxDistanceFld     = addFloat("averageMaximumDistance", 0.0f );

  //////////////////////////////////////////////////////////////////////////

  _tolleranceFld = addDouble("tollerance", 0.0001 );

  //////////////////////////////////////////////////////////////////////////

  _applyFld = fieldC->addNotify("apply");
  (_autoApplyFld = fieldC->addBool("autoApply"))->setBoolValue(true);
  _statusFld = fieldC->addString("status");
  _statusFld->setStringValue("Ready");

  //////////////////////////////////////////////////////////////////////////

  (_listenToRepaintNotificationsFld          = fieldC->addBool("listenToRepaintNotifications"))
    ->setBoolValue(false);
  (_listenToFinishingNotificationsFld        = fieldC->addBool("listenToFinishingNotifications"))
    ->setBoolValue(true);
  (_listenToSelectionChangedNotificationsFld = fieldC->addBool("listenToSelectionChangedNotifications"))
    ->setBoolValue(false);

  char* statisticsNames[LASTSTATISTIC];
  statisticsNames[ MIN ] = "Minimum";
  statisticsNames[ MAX ] = "Maximum";
  statisticsNames[ MEAN ] = "Mean";
  statisticsNames[ STDEV ] = "StDev";
  _curveStatistic = addEnum("curveStatistic",statisticsNames,LASTSTATISTIC );

  _isInNotificationCB = false; 	
  
  ML_CHECK_NEW(_outputXMarkerList,XMarkerList());

	(_outputXMarkerListFld = getFieldContainer()->addBase("outputXMarkerList"))->setBaseValue(NULL);

  _outputCurveList = new CurveList;
  (_outputCurveListFld = getFieldContainer()->addBase("outputCurveList"))->setBaseValue( _outputCurveList );

  handleNotificationOn();
}

//////////////////////////////////////////////////////////////////////////

CSODistance::~CSODistance()
{
  ML_TRACE_IN("CSODistance::~CSODistance()");

  if (_csoList0 != NULL){
    _csoList0->removeNotificationObserver(_csoListNotificationCB, this);
  }

  ML_DELETE(_outputXMarkerList);

  // Delete all created CurveData objects
  while ( !_outputCurveList->getCurveList().empty() ) {
    ML_DELETE( _outputCurveList->getCurveList().back() );
    _outputCurveList->getCurveList().pop_back();
  }
  ML_DELETE( _outputCurveList );

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

  if (field == _input0CSOFld){

    if (!_isInNotificationCB && _csoList0 ) { _csoList0->removeNotificationObserver(_csoListNotificationCB, this);}

    if (BASE_IS_A((static_cast<Base*>(_input0CSOFld->getBaseValue())), CSOList)){

      _csoList0 = static_cast<CSOList*>(_input0CSOFld->getBaseValue());

      if (_csoList0 != NULL){

        if (!_isInNotificationCB) { _csoList0->addNotificationObserver(_csoListNotificationCB, this); }
      }
    } else {

      _csoList0 = NULL;
    }

    if (_autoApplyFld->getBoolValue()) { _process(); }
  } else
  if (field == _input1CSOFld){

    if (!_isInNotificationCB && _csoList1 ) { _csoList1->removeNotificationObserver(_csoListNotificationCB, this);}

    if (BASE_IS_A((static_cast<Base*>(_input1CSOFld->getBaseValue())), CSOList)){

      _csoList1 = static_cast<CSOList*>(_input1CSOFld->getBaseValue());

      if (_csoList1 != NULL){

        if (!_isInNotificationCB) { _csoList1->addNotificationObserver(_csoListNotificationCB, this); }
      }
    } else {

      _csoList1 = NULL;
    }

    if (_autoApplyFld->getBoolValue()) { _process(); }
  } else
  if ( field == _applyFld || ( _autoApplyFld->getBoolValue() && 
                             ( field == _tolleranceFld || field == _curveStatistic )) ){
    _process();
  }


  BaseOp::handleNotification(field);
}

//////////////////////////////////////////////////////////////////////////

void CSODistance::_process()
{   
  ML_TRACE_IN("void CSODistance::_process()");

  _outputXMarkerList->clearList();

  // Delete all created CurveData objects
  while ( !_outputCurveList->getCurveList().empty() ) {
    ML_DELETE( _outputCurveList->getCurveList().back() );
    _outputCurveList->getCurveList().pop_back();
  }

  if (_csoList0 != NULL) {

    int nCSOs = _csoList0->numCSO();

    double minDistance = 0.0;
    double maxDistance = 0.0;
    double avgDistance = 0.0;
    double stdDevDistance = 0.0;

    std::stringstream distances;
    distances << _tableHeader  << std::endl;


    switch ( _modeFld->getEnumValue() ){
      case FIRST2 :
      {
        if ((nCSOs >= 2) && (_csoList0->getCSOAt(0)->getIsFinished()) && (_csoList0->getCSOAt(1)->getIsFinished())) {
          std::vector<vec3>pointSet1;
          std::vector<vec3>pointSet2;
          
          _csoList0->getCSOAt(0)->fillPathPointCoordinatesFlattened(pointSet1);
          _csoList0->getCSOAt(1)->fillPathPointCoordinatesFlattened(pointSet2);

           Vector3 minPoint1,minPoint2,maxPoint1,maxPoint2;

           MinimalDistancePointClouds* pointSetsMinDist = NULL;
           ML_CHECK_NEW(pointSetsMinDist, MinimalDistancePointClouds);
           
           pointSetsMinDist->setPointSets(pointSet1, pointSet2);
           pointSetsMinDist->setNumEntries(200);
           pointSetsMinDist->computeDistance(minPoint1,minPoint2);

           minDistance = sqrt((minPoint1[0]-minPoint2[0])*(minPoint1[0]-minPoint2[0]) + 
                              (minPoint1[1]-minPoint2[1])*(minPoint1[1]-minPoint2[1]) + 
                              (minPoint1[2]-minPoint2[2])*(minPoint1[2]-minPoint2[2]));
                    


          _minimumDistanceFld->setFloatValue( static_cast<float>(minDistance));

          _outputXMarkerList->appendItem(XMarker( vec6(minPoint1[0],minPoint1[1],minPoint1[2],0.5f,0.0f,0.0f),
                                                  vec3(minPoint2[0]-minPoint1[0],minPoint2[1]-minPoint1[1],minPoint2[2]-minPoint1[2]),
                                                  0,""));
          distances << _csoList0->getCSOAt(0)->getId() << "," 
                    << _csoList0->getCSOAt(1)->getId() << "," 
                    << minDistance << "," 
                    << maxDistance << "," 
                    << avgDistance << "," 
                    << stdDevDistance 
                    << std::endl;
        } else {
          _minimumDistancePoint1Fld->setVec3fValue(vec3(0.0,0.0,0.0));
          _minimumDistancePoint2Fld->setVec3fValue(vec3(0.0,0.0,0.0));
          _minimumDistanceFld->setFloatValue(0.0f);
          _distancesFld->setStringValue( _tableHeader );
        }
        break;
      }
      case INPLANE:
      case INDEX:
      {
        CurveData *outputCurve = new CurveData;
        double    *yValues     = new double[ nCSOs ];

        double minDist = ML_DOUBLE_MAX;
        vec3 point1;
        vec3 point2;
        float averageMinDistance = 0.0f;
        float averageMeanDistance = 0.0f;
        float averageMaxDistance = 0.0f;
        for ( int iCSO = 0; iCSO<nCSOs; ++iCSO ){
          CSO* currentCSO = _csoList0->getCSOAt( iCSO );
          CSO* matchingCSO = _findMatchingCSO(iCSO);
          if (!matchingCSO) {continue;}

          std::vector<vec3>pointSet1;
          std::vector<vec3>pointSet2;
          currentCSO->fillPathPointCoordinatesFlattened(pointSet1);
          matchingCSO->fillPathPointCoordinatesFlattened(pointSet2);

          Vector3 minPoint1,minPoint2,maxPoint1,maxPoint2;

          _getDistances( pointSet1, pointSet2,
                         minDistance,maxDistance,avgDistance,stdDevDistance,
                         minPoint1,minPoint2,maxPoint1,maxPoint2);

          averageMinDistance  += minDistance;
          averageMeanDistance += avgDistance;
          averageMaxDistance  += maxDistance;

          distances << currentCSO->getId()  << "," 
                    << matchingCSO->getId() << "," 
                    << minDistance << "," 
                    << maxDistance << "," 
                    << avgDistance << "," 
                    << stdDevDistance 
                    << std::endl;

          if ( minDistance < minDist ){
            minDist = minDistance;
            point1 = minPoint1;
            point2 = minPoint2;
          }

          _outputXMarkerList->appendItem(XMarker( vec6(minPoint1[0],minPoint1[1],minPoint1[2],0.5f,0.0f,0.0f),
                                                  vec3(minPoint2[0]-minPoint1[0],minPoint2[1]-minPoint1[1],minPoint2[2]-minPoint1[2]),
                                                  currentCSO->getId(),""));

          switch ( _curveStatistic->getEnumValue() ){
            case MIN:
              yValues[ iCSO ] = minDistance;
              break;
            case MAX:
              yValues[ iCSO ] = maxDistance;
              break;
            case MEAN:
              yValues[ iCSO ] = avgDistance;
              break;
            case STDEV:
              yValues[ iCSO ] = stdDevDistance;
              break;
            default:
              break;
          } 
        } // iCSO
        averageMinDistance  /= (nCSOs != 0 ? nCSOs : 1.0);
        averageMeanDistance /= (nCSOs != 0 ? nCSOs : 1.0);
        averageMaxDistance  /= (nCSOs != 0 ? nCSOs : 1.0);

        outputCurve->setY( nCSOs, yValues);
        delete[] yValues;
        _outputCurveList->getCurveList().push_back( outputCurve );
        _distancesFld->setStringValue( distances.str() );
        _minimumDistancePoint1Fld->setVec3fValue(point1);
        _minimumDistancePoint2Fld->setVec3fValue(point2);
        _minimumDistanceFld->setFloatValue( static_cast<float>(minDistance) );

        _AverageMinimumDistanceFld->setFloatValue( averageMinDistance  );
        _AverageMeanDistanceFld->setFloatValue(    averageMeanDistance );
        _AverageMaxDistanceFld->setFloatValue(     averageMaxDistance  );



        break;
      }
    default:
      break;
    }

  } else {
    _minimumDistancePoint1Fld->setVec3fValue(vec3(0.0,0.0,0.0));
    _minimumDistancePoint2Fld->setVec3fValue(vec3(0.0,0.0,0.0));
    _minimumDistanceFld->setFloatValue(0.0f);
    _distancesFld->setStringValue( _tableHeader );
  }

  _outputXMarkerListFld->setBaseValue(_outputXMarkerList);
  _outputCurveListFld->touch();

}

//////////////////////////////////////////////////////////////////////////

void CSODistance::_getDistances( std::vector<Vector3> &set1, 
                                 std::vector<Vector3> &set2,
                                 double               &minDistance, 
                                 double               &maxDistance, 
                                 double               &averageDistance, 
                                 double               &stdDevDistance,
                                 Vector3              &minPoint1,
                                 Vector3              &minPoint2,
                                 Vector3              &maxPoint1,
                                 Vector3              &maxPoint2)
{


  minDistance = ML_DOUBLE_MAX;
  maxDistance = 0.0;
  double sumDist = 0.0;
  double sqrDist = 0.0;
  for(int i = 0; i < int(set1.size()); i++)
  {
    Vector3 pos1 = set1[i];
    Vector3 pos2;
    double currentDistance = ML_DOUBLE_MAX;
    for(int j = 0; j < int( set2.size() ); j++)
    {
      Vector3 pos = set2[j];
      double dist = (pos1 - pos).length();
      if ( dist < currentDistance ){
        currentDistance = dist;
        pos2 = pos;
      }
    }

    if ( minDistance > currentDistance ){
      minDistance = currentDistance;
      minPoint1 = pos1;
      minPoint2 = pos2;
    }
    if (maxDistance < currentDistance ){
      maxDistance = currentDistance;
      maxPoint1 = pos1;
      maxPoint2 = pos2;
    }
    sumDist += currentDistance;
    sqrDist += currentDistance*currentDistance;
  }
  double nPoints = ( set1.size() ? static_cast<double>(set1.size()) : 1.0);
  averageDistance = sumDist/nPoints;
  stdDevDistance = sqrDist/nPoints - averageDistance*averageDistance;
}

//////////////////////////////////////////////////////////////////////////

CSO* CSODistance::_findMatchingCSO(int findCSO)
{
  CSO* matchingCSO = NULL;

  if ( _modeFld->getEnumValue() == INDEX && _csoList0 != NULL && _csoList1 != NULL) {
    if (findCSO < static_cast<int>( _csoList1->numCSO()) ){
      matchingCSO = _csoList1->getCSOAt( findCSO );
    }
  } else {

    const int nCSOs          = _csoList0->numCSO();
    const CSO* currentCSO    = _csoList0->getCSOAt( findCSO );
    const double tollerance  = _tolleranceFld->getDoubleValue();
    vec3 currentNormal = currentCSO->getPlaneNormal();
    currentNormal.normalize();
    const vec3 currentPoint  = currentCSO->getFirstSeedPoint()->worldPosition;

    double minError = ML_DOUBLE_MAX;
    for ( int iCSO = 0; iCSO<nCSOs; ++iCSO ){
      CSO* cso = _csoList0->getCSOAt( iCSO );
      if ( cso == currentCSO ) { continue; }
      vec3 normal = cso->getPlaneNormal();
      normal.normalize();
      const vec3 matchingPoint = cso->getFirstSeedPoint()->worldPosition;
      const double angleError = 1- abs( currentNormal.dot( normal ));
      const double distanceError = abs(currentNormal.dot( matchingPoint - currentPoint ));
      const double currentError =  angleError + distanceError; 
      if (  currentError < minError && angleError < tollerance && distanceError < 10*tollerance ) {
        minError = currentError;
        matchingCSO = cso;
      }
    }
  }

  return matchingCSO;
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

