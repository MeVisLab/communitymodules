//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDBoundingBox2Init.cpp
// \author  Konrad Mühler
// \date    2005-05-10
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __UMDBoundingBox2System_H
#include "UMDBoundingBox2System.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#ifndef __mlUMDBoundingBox2_H
#include "mlUMDBoundingBox2.h"
#endif


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int UMDBoundingBox2Init (void)
{
  ML_TRACE_IN("UMDBoundingBox2Init ()")

  UMDBoundingBox2::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(UMDBoundingBox2Init)

