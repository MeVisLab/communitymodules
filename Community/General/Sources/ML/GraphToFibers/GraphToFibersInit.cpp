//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    GraphToFibersInit.cpp
// \author  Axel Newe
// \date    2016-03-31
*/
//----------------------------------------------------------------------------------


// Local includes
#include "GraphToFibersSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "mlGraphToFibers.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int GraphToFibersInit()
{
  // Add initClass calls from modules here.
  GraphToFibers::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(GraphToFibersInit)