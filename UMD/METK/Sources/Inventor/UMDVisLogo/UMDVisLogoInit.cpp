//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDVisLogoInit.cpp
// \author  Konrad Mühler
// \date    2007-06-12
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDVisLogoSystem.h"
#include <XVBasics.h>
#include "SoVisLogo.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDVisLogoInit()
{
  // Register the class in the inventor runtime type system.
  SoVisLogo::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDVisLogoInit)


