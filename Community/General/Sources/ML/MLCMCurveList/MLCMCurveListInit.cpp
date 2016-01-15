//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLCMCurveListInit.cpp
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLCMCurveListSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...


#include "CMCurveFilter/mlCMCurveFilter.h"
#include "StringToCurve/mlStringToCurve.h"
#include "CurveToString/mlCurveToString.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int MLCMCurveListInit (void)
{
  ML_TRACE_IN("MLCMCurveListInit ()")

  // Add initClass calls from all other modules here...

  CMCurveFilter::initClass();
  StringToCurve::initClass();
  CurveToString::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLCMCurveListInit)







