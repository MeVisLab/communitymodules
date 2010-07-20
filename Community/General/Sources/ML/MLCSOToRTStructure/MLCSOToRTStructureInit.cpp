//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLCSOToRTStructureInit.cpp
// \author  linal
// \date    2010-01-20
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLCSOToRTStructureSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlCSOToRTStructure.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLCSOToRTStructureInit ()
{
  ML_TRACE_IN("MLCSOToRTStructureInit ()")

  CSOToRTStructure::initClass();
  // Add initClass calls from all other modules here...
  
  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLCSOToRTStructureInit)

