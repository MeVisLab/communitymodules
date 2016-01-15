//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLCMCoordinateInit.cpp
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLCMCoordinateSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...


#include "WorldInfo/mlWorldInfo.h"
#include "BoxArithmetic/mlBoxArithmetic.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int MLCMCoordinateInit (void)
{
  ML_TRACE_IN("MLCMCoordinateInit ()")

  // Add initClass calls from all other modules here...

  WorldInfo::initClass();
  BoxArithmetic::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLCMCoordinateInit)







