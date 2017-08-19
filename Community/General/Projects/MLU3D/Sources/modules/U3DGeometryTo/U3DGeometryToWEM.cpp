//----------------------------------------------------------------------------------
// U3DGeometryToWEM module.
//
// \file    U3DGeometryToWEM.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DGeometryToWEM.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DGeometryToWEM, U3DInspector);


//***********************************************************************************


U3DGeometryToWEM::U3DGeometryToWEM() : U3DInspector()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  ML_CHECK_NEW(_outWEM, WEM());

  // Output fields and allowed types.
  (_outMeshWEMFld          = addBase("outMeshWEM"))->setBaseValueAndAddAllowedType(_outWEM);

  // Add and initialize parameter fields
  _autoApplyFld->setBoolValue(true);


  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DGeometryToWEM::~U3DGeometryToWEM()
{
  // Destroy own dynamic data structures here
  if (_outWEM)
  {
    _outWEM->removeAll();
  }

  _outWEM = NULL;

}


//***********************************************************************************


void U3DGeometryToWEM::handleNotification (Field* field)
{
  // Call parent class first to make sure that input object is set correctly.
  U3DInspector::handleNotification(field);
}


//***********************************************************************************


void U3DGeometryToWEM::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DInspector::activateAttachments();
}


//***********************************************************************************


void U3DGeometryToWEM::process()
{
  if (_inU3DValid)
  {
    mlU3D::U3DMarkerListTools::fillWEMFromMeshGenerators(_outWEM, _inU3DObject->meshes);
    _networkPanelStatusFld->setStringValue("Extracted " + mlU3D::intToString((int)_inU3DObject->meshes.size()) + " mesh(es).");
  }
  else
  {
    _outWEM->removeAll();
    _networkPanelStatusFld->setStringValue("No valid U3D connected.");
  }


  _outMeshWEMFld->touch();
}


//***********************************************************************************


ML_END_NAMESPACE