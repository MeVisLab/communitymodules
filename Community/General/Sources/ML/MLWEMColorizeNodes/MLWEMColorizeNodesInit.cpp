//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    
// \author  Axel Newe
// \date    2015-12-03
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLWEMColorizeNodesSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "WEMColorizeNodes.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLWEMColorizeNodesInit()
{
  // Add initClass calls from modules here.
  WEMColorizeNodes::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLWEMColorizeNodesInit)