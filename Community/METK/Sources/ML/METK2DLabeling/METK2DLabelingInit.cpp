//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METK2DLabelingInit.cpp
// \author  Konrad Mühler
// \date    2008-04-11
*/
//----------------------------------------------------------------------------------


// Local includes
#include "METK2DLabelingSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlMETK2DLabeling.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int METK2DLabelingInit (void)
{
  ML_TRACE_IN("METK2DLabelingInit ()")

  METK2DLabeling::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(METK2DLabelingInit)