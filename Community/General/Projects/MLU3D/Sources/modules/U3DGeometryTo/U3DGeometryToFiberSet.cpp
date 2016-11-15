//----------------------------------------------------------------------------------
// U3DGeometryToFiberSet module.
//
// \file    U3DGeometryToFiberSet.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DGeometryToFiberSet.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DGeometryToFiberSet, U3DInspector);


//***********************************************************************************


U3DGeometryToFiberSet::U3DGeometryToFiberSet() : U3DInspector()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Output fields and allowed types.
  (_outLineSetFibersFld    = addBase("outLineSetFibers"))->setBaseValueAndAddAllowedType(&_outFiberSetContainer);

  // Add and initialize parameter fields
  _autoApplyFld->setBoolValue(true);


  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DGeometryToFiberSet::~U3DGeometryToFiberSet()
{
  // Destroy own dynamic data structures here
  _outFiberSetContainer.deleteAllFiberSets();
}


//***********************************************************************************


void U3DGeometryToFiberSet::handleNotification (Field* field)
{
  // Call parent class first to make sure that input object is set correctly.
  U3DInspector::handleNotification(field);
}


//***********************************************************************************


void U3DGeometryToFiberSet::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DInspector::activateAttachments();
}


//***********************************************************************************


void U3DGeometryToFiberSet::process()
{
  if (_inU3DValid)
  {
    mlU3D::U3DMarkerListTools::fillFiberSetContainerFromLineSetGenerators(_outFiberSetContainer, _inU3DObject->lineSets);
    _networkPanelStatusFld->setStringValue("Extracted " + mlU3D::intToString((int)_inU3DObject->lineSets.size()) + " line set(s).");
  }
  else
  {
    _outFiberSetContainer.deleteAllFiberSets();
    _networkPanelStatusFld->setStringValue("No valid U3D connected.");
  }
  _outLineSetFibersFld->touch();
}


//***********************************************************************************


ML_END_NAMESPACE