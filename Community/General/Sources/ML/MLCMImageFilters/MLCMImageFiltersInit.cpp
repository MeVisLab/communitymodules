//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLCMImageFilters.cpp
// \author  Theo van Walsum
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLCMImageFiltersSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "ExpressionParser/mlNewImageFromExpression.h"
#include "ExpressionParser/mlExpressionParser1.h"
#include "ExpressionParser/mlExpressionParser2.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int MLCMImageFiltersInit (void)
{
  ML_TRACE_IN("MLCMImageFilters ()")

  NewImageFromExpression::initClass();
  ExpressionParser1::initClass();
  ExpressionParser2::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLCMImageFiltersInit)

