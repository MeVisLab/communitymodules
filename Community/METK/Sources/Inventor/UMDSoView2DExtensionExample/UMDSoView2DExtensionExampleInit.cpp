//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDSoView2DExtensionExampleInit.cpp
// \author  Konrad Mühler
// \date    2008-01-07
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDSoView2DExtensionExampleSystem.h"
#include <XVBasics.h>
#include "SoView2DExtensionExample.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDSoView2DExtensionExampleInit()
{
  // Register the class in the inventor runtime type system.
  SoView2DExtensionExample::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDSoView2DExtensionExampleInit)


