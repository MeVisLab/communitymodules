//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDSoSceneWriterMDInit.cpp
// \author  Konrad Mühler
// \date    2006-05-15
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __UMDSoSceneWriterMDSystem_H
#include "UMDSoSceneWriterMDSystem.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#ifndef __mlSoSceneWriterMD_H
#include "mlSoSceneWriterMD.h"
#endif


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int UMDSoSceneWriterMDInit (void)
{
  ML_TRACE_IN("UMDSoSceneWriterMDInit ()")

  SoSceneWriterMD::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(UMDSoSceneWriterMDInit)

