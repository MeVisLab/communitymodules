//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDBoundingBoxCalculator2Init.cpp
// \author  Konrad
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __UMDBoundingBoxCalculator2System_H
#include "UMDBoundingBoxCalculator2System.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#ifndef __mlUMDBoundingBoxCalculator2_H
#include "mlUMDBoundingBoxCalculator2.h"
#endif


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int UMDBoundingBoxCalculator2Init (void)
{
  ML_TRACE_IN("UMDBoundingBoxCalculator2Init ()")

  UMDBoundingBoxCalculator2::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(UMDBoundingBoxCalculator2Init)

