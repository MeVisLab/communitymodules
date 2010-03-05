// **InsertLicense** code
//----------------------------------------------------------------------------------
//!
/*!
// \file    MLCSOCommunityModulesInit.cpp
// \author  Bart De Dobbelaer
// \date    05/2009
*/
//----------------------------------------------------------------------------------

#include "MLCSOCommunityModulesSystem.h"

#include <mlLibraryInitMacros.h>

#include "CSODistance/CSODistance.h"
#include "CSOImageStatistics/mlCSOImageStatistics.h"
#include "CSOSetProperties/mlCSOSetProperties.h"
#include "CSOPointsToXMarkers/mlCSOPointsToXMarkers.h"

ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

int MLCSOCommunityModulesInit() {

  ML_TRACE_IN("MLCSOCommunityModulesInit()");

  CSODistance::initClass();
  CSOImageStatistics::initClass();
  CSOSetProperties::initClass();
  CSOPointsToXMarkers::initClass();

  return 1;
}

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE

ML_INIT_LIBRARY(MLCSOCommunityModulesInit)
