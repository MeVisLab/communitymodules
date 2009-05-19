//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLTestModulesInit.cpp
// \author  Wolf Spindler
// \date    2009-05-19
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLTestModulesSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlNOP.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLTestModulesInit ()
{
  ML_TRACE_IN("MLTestModulesInit ()")

  NOP::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLTestModulesInit)

