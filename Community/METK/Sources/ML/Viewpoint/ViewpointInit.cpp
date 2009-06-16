//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    ViewpointInit.cpp
// \author  Konrad Mühler
// \date    2007-07-26
*/
//----------------------------------------------------------------------------------


// Local includes
#include "ViewpointSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "METKCreateIntraOPDummy/mlMETKCreateIntraOPDummy.h"
#include "METKIntraOPVIewpoint/mlMETKIntraOPViewpoint.h"
#include "METKAutoFading/mlMETKAutoFading.h"
#include "METKCalcCamPos/METKCalcCamPos.h"
#include "METKShowClusteredObjects/METKShowClusteredObjects.h"
#include "METKCalcCamPath/METKCalcCamPath.h"
#include "METKToSolverConnection/METKToSolverConnection.h"
#include "METKKeystates/METKKeystates.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int ViewpointInit (void)
{
  ML_TRACE_IN("ViewpointInit ()")

  METKCreateIntraOPDummy::initClass();
  METKIntraOPViewpoint::initClass();
  METKAutoFading::initClass();
  METKCalcCamPos::initClass();
  METKShowClusteredObjects::initClass();
  METKToSolverConnection::initClass();
  METKCalcCamPath::initClass();
  METKKeystates::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(ViewpointInit)

