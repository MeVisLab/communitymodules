//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDSoExtractColoredFacesInit.cpp
// \author  Konrad Mühler
// \date    2008-02-01
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDSoExtractColoredFacesSystem.h"
#include <XVBasics.h>
#include "SoExtractColoredFaces.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDSoExtractColoredFacesInit()
{
  // Register the class in the inventor runtime type system.
  SoExtractColoredFaces::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDSoExtractColoredFacesInit)


