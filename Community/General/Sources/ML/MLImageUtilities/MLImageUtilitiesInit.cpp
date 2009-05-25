
#include "MLImageUtilitiesSystem.h"
#include "mlLibraryInitMacros.h"

#include "PlaneClip/PlaneClip.h"
#include "AnalyzeHeader/AnalyzeHeader.h"
#include "MIP/MIP.h"

ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

int MLImageUtilitiesInit() {

  ML_TRACE_IN("MLImageUtilitiesInit()");

  PlaneClip     ::initClass();
  AnalyzeHeader ::initClass();
	MIP           ::initClass();

  return 1;
}

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE

ML_INIT_LIBRARY(MLImageUtilitiesInit)
