//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDRegionDistanceTransformationInit.cpp
// \author  Konrad Muehler
// \date    2009-03-17
*/
//----------------------------------------------------------------------------------


// Local includes
#include "UMDRegionDistanceTransformationSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlVirtualVolumeAdapter.h"

// to small image interface
#include "SmallImageInterface.h"

// Include all module headers ...
#include "mlRegionDistanceTransformation.h"

//! each SmallImageInterface class needs to be declared
SMALLIMAGEINTERFACE_DECLARE(RegionDistanceTransformation)

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int UMDRegionDistanceTransformationInit (void)
{
  ML_TRACE_IN("UMDRegionDistanceTransformationInit ()")

  // initialize the SmallImageInterface classes
  SMALLIMAGEINTERFACE_INIT(RegionDistanceTransformation);

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(UMDRegionDistanceTransformationInit)

