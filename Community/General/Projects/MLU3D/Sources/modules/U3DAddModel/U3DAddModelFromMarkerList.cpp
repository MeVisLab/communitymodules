//----------------------------------------------------------------------------------
// U3DAddModelFromMarkerList module.
//
// \file    U3DAddModelFromMarkerList.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAddModelFromMarkerList.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_Constants.h"
#include "../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE

using namespace mlU3D;

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DAddModelFromMarkerList, U3DProcessor);


//***********************************************************************************

U3DAddModelFromMarkerList::U3DAddModelFromMarkerList() : U3DProcessor()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.
  // Add input fields to the module and set allowed types.
  (_inPositionsFld = addBase("inPositions"))->setBaseValueAndAddAllowedType(&_inPositions);
  _inPositionsFld->addAllowedType<ml::ColoredMarkerList>();

  // Add and initialize parameter fields
  (_simpleModePointSetFld      = addBool("simpleModePointSet"))->setBoolValue(false);
  (_pointCloudSpecificationFld = addString("pointCloudSpecification"))->setStringValue("");

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

U3DAddModelFromMarkerList::~U3DAddModelFromMarkerList()
{
  // Destroy own dynamic data structures here
}

//***********************************************************************************

void U3DAddModelFromMarkerList::handleNotification(Field* field)
{
  if (field == _inPositionsFld)
  {
    ml::Base *inValue = _inPositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::ColoredMarkerList))
      {
        ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)inValue);
        _inPositions = inList->toXMarkerList();
      }
      else
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
        _inPositions = *inList;
      }

    }
    else
    {
      _inPositions.clear();
    }

    if (_autoApplyFld->getBoolValue())
    {
      process();
    }

  }

  // call parent class and handle apply/autoApply and in/outputs
  U3DProcessor::handleNotification(field);
}

//***********************************************************************************

void U3DAddModelFromMarkerList::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DProcessor::activateAttachments();
}

//***********************************************************************************

void U3DAddModelFromMarkerList::process()
{
  // Execute module functionaylity here.
  if (_inU3DValid)
  {
    _addPointSetModelData();
    _outU3DObject->updateGroupNodes();
    _networkPanelStatusFld->setStringValue(_outU3DObject->getNetworkPanelInfo());
  }
  else
  {
    _networkPanelStatusFld->setStringValue("U3D Input invalid");
  }

  _outU3DObjectFld->touch();
}

//***********************************************************************************

void U3DAddModelFromMarkerList::_addPointSetModelData()
{
  StringVector pointSetSpecificationsVector;

  if (_simpleModePointSetFld->getBoolValue())
  {
    pointSetSpecificationsVector.push_back("<PointSet><PositionTypes>all</PositionTypes></PointSet>");
  }
  else
  {
    pointSetSpecificationsVector = U3DTools::getModelSpecificationsStringFromUI(_pointCloudSpecificationFld, "<PointSet>");
  }

  if (0 == pointSetSpecificationsVector.size())
  {
    return;
  }

  U3DTools::addPointSetModelAndGeometry(pointSetSpecificationsVector, _outU3DObject, _inPositions);
}

//***********************************************************************************


ML_END_NAMESPACE