//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDsplitCurveListInit.cpp
// \author  Tobias Mönch
// \date    2009-08-05
*/
//----------------------------------------------------------------------------------


// Local includes
#include "UMDsplitCurveListSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlUMDsplitCurveList.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int UMDsplitCurveListInit ()
{
  ML_TRACE_IN("UMDsplitCurveListInit ()")

  UMDsplitCurveList::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(UMDsplitCurveListInit)

