//----------------------------------------------------------------------------------
// U3DGeometryToXMarkerList module.
//
// \file    U3DGeometryToXMarkerList.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DGeometryToXMarkerList.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DGeometryToXMarkerList, U3DInspector);


//***********************************************************************************


U3DGeometryToXMarkerList::U3DGeometryToXMarkerList() : U3DInspector()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Output fields and allowed types.
  (_outPointSetXMarkersFld = addBase("outPointSetXMarkers"))->setBaseValueAndAddAllowedType(&_outPointPositions);

  // Add and initialize parameter fields
  _autoApplyFld->setBoolValue(true);

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DGeometryToXMarkerList::~U3DGeometryToXMarkerList()
{
  // Destroy own dynamic data structures here
  _outPointPositions.clearList();
}


//***********************************************************************************


void U3DGeometryToXMarkerList::handleNotification (Field* field)
{
  // Call parent class first to make sure that input object is set correctly.
  U3DInspector::handleNotification(field);
}


//***********************************************************************************


void U3DGeometryToXMarkerList::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DInspector::activateAttachments();
}


//***********************************************************************************


void U3DGeometryToXMarkerList::process()
{
  if (_inU3DValid)
  {
    mlU3D::U3DMarkerListTools::fillXMarkerListFromPointSetGenerators(_outPointPositions, _inU3DObject->pointSets);
    _networkPanelStatusFld->setStringValue("Extracted " + mlU3D::intToString((int)_inU3DObject->pointSets.size()) + " point set(s).");
  }
  else
  {
    _outPointPositions.clearList();
    _networkPanelStatusFld->setStringValue("No valid U3D connected.");
  }

  _outPointSetXMarkersFld->touch();
}


//***********************************************************************************


ML_END_NAMESPACE