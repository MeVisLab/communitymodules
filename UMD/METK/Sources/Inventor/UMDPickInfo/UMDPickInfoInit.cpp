//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDPickInfoInit.cpp
// \author  Stefan Hiller
// \date    2007-09-11
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDPickInfoSystem.h"
#include <XVBasics.h>
#include "SoPickInfo.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDPickInfoInit()
{
  // Register the class in the inventor runtime type system.
  SoPickInfo::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDPickInfoInit)


