//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    LabelingInit.cpp
// \author  Konrad Mühler
// \date    2007-07-13
*/
//----------------------------------------------------------------------------------


// Local includes
#include "LabelingSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "METKLabels/mlMETKLabels.h"
#include "UMDLabelModul/SoLabelModul.h"
#include "UMDTextModul/SoTextModul.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int LabelingInit (void)
{
  ML_TRACE_IN("LabelingInit ()")

  METKLabels::initClass();
  SoLabelModul::initClass();
  SoTextModul::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(LabelingInit)