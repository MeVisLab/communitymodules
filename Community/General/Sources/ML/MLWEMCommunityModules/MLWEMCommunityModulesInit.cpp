// **InsertLicense** code
//----------------------------------------------------------------------------------
//!
/*!
// \file    MLWEMCommunityModulesInit.cpp
// \author  Bart De Dobbelaer
// \date    05/2009
*/
//----------------------------------------------------------------------------------

#include "MLWEMCommunityModulesSystem.h"

#include <mlLibraryInitMacros.h>

#include "WEMPlane/WEMPlane.h"
#include "WEMCenterOfMass/WEMCenterOfMass.h"
#include "WEMVolume/WEMVolume.h"
#include "WEMCMSelectPatches/WEMCMSelectPatches.h"
#include "WEMNodesToFile/WEMNodesToFile.h"

ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

int MLWEMCommunityModulesInit() {

  ML_TRACE_IN("MLWEMCommunityModulesInit()");

  WEMPlane::initClass();
  WEMVolume::initClass();
  WEMCMSelectPatches::initClass();
  WEMNodesToFile::initClass();
  WEMCenterOfMass::initClass();

  return 1;
}

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE

ML_INIT_LIBRARY(MLWEMCommunityModulesInit)
