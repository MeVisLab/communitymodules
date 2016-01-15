//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    CVLAscendingAortaDetectionInit.cpp
// \author  Stefan Saur
// \date    2007-03-24
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __CVLAscendingAortaDetectionSystem_H
#include "CVLAscendingAortaDetectionSystem.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#ifndef __mlAscendingAortaDetection_H
#include "mlAscendingAortaDetection.h"
#endif

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int CVLAscendingAortaDetectionInit (void)
{
  ML_TRACE_IN("CVLAscendingAortaDetectionInit ()")

  AscendingAortaDetection::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(CVLAscendingAortaDetectionInit)

