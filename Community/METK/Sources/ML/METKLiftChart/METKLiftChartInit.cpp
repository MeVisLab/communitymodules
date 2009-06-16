//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDInventorPointerInit.cpp
// \author  Konrad
// \date    2006-02-02
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __METKLIFTCHARTSYSTEM_H
#include "METKLiftChartSystem.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#ifndef __MLMETKLIFTCHART_H
#include "mlMETKLiftChart.h"
#endif
#include "SoView2DLiftChart.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int METKLiftChartInit (void)
{
  ML_TRACE_IN("METKLiftChartInit ()")

  METKLiftChart::initClass();
  SoView2DLiftChart::initClass();  
  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(METKLiftChartInit)

