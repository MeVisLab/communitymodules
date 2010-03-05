//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLXMarkerListCommunityModulesInit.cpp
// \author  Coert Metz
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLXMarkerListCommunityModulesSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "InterpolateXMarkerList/mlLinearInterpolateXMarkerList.h"
#include "TransformXMarkerList/mlTransformXMarkerList.h"
#include "XMarkerListFile/mlXMarkerListFromFile.h"
#include "XMarkerListFile/mlXMarkerListToFile.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLXMarkerListCommunityModulesInit ()
{
  ML_TRACE_IN("MLXMarkerListCommunityModulesInit ()")

  LinearInterpolateXMarkerList::initClass();
  TransformXMarkerList::initClass();
  XMarkerListFromFile::initClass();
  XMarkerListToFile::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLXMarkerListCommunityModulesInit)

