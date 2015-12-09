//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    PDF3DWorkInit.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-20
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDF3DWorkSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "PDF3DWork.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int PDF3DWorkInit()
{
  // Add initClass calls from modules here.
  PDF3DWork::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(PDF3DWorkInit)