//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDStipplingInit.cpp
// \author  Alexandra Baer
// \date    2006-07-26
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDStipplingInit.h"
#include <XVBasics.h>
#include "SoStippling.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDStipplingInit()
{
  // Register the class in the inventor runtime type system.
  SoStippling::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDStipplingInit)


