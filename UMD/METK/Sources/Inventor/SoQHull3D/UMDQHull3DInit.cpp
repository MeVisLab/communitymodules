//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDQHull3DInit.cpp
// \author  Jens Haase
// \date    2005-02-01
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDQHull3DSystem.h"
#include <XVBasics.h>
#include "SoQHull3D.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDQHull3DInit()
{
  // Register the class in the inventor runtime type system.
  SoQHull3D::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDQHull3DInit)

