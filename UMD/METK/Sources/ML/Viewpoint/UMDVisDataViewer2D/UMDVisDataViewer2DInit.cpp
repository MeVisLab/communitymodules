//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDVisDataViewer2DInit.cpp
// \author
// \date    2007-04-26
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDVisDataViewer2DSystem.h"
#include <XVBasics.h>
#include "SoVisDataViewer2D.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDVisDataViewer2DInit()
{
  // Register the class in the inventor runtime type system.
  SoVisDataViewer2D::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDVisDataViewer2DInit)


