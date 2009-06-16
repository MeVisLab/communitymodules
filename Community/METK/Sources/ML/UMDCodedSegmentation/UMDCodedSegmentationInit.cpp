//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDCodedSegmentationInit.cpp
// \author  Konrad Mühler
// \date    2007-01-16
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __UMDCodedSegmentationSystem_H
#include "UMDCodedSegmentationSystem.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#ifndef __mlCalcCodedSegmentation_H
#include "mlCalcCodedSegmentation.h"
#endif


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int UMDCodedSegmentationInit (void)
{
  ML_TRACE_IN("UMDCodedSegmentationInit ()")

  CalcCodedSegmentation::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(UMDCodedSegmentationInit)

