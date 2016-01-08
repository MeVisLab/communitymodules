//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLVectorcardiogramSystem.cpp
// \author  Axel Newe
// \date    2015-09-25
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLVectorcardiogramSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "VCGLoad/VCGLoad.h"
#include "VCGCoordinateSystem/VCGCoordinateSystem.h"
#include "VCGLineSetDefinition/VCGLineSetDefinition.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLVectorcardiogramInit()
{
  ML_TRACE_IN("MLVectorcardiogramInit()")
  
  // Add initClass calls from modules here.
  VCGLoad             ::initClass();
  VCGCoordinateSystem ::initClass();
  VCGLineSetDefinition::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLVectorcardiogramInit)