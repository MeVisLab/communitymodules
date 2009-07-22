//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    CVLSegmentationEvaluationMetricInit.cpp
// \author  Stefan Saur
// \date    2009-07-21
*/
//----------------------------------------------------------------------------------


// Local includes
#include "CVLSegmentationEvaluationMetricSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlSegmentationEvaluationMetric.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int CVLSegmentationEvaluationMetricInit ()
{
  ML_TRACE_IN("CVLSegmentationEvaluationMetricInit ()")

  SegmentationEvaluationMetric::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(CVLSegmentationEvaluationMetricInit)

