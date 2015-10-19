//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDDraw2DSilhouetteInit.cpp
// \author  Björn Meyer
// \date    2004-09-28
//
*/
//----------------------------------------------------------------------------------

// Local includes
#pragma warning( push, 1 )
#include "UMDSoView2DExtensionsSystem.h"
#include <XVBasics.h>
#pragma warning( pop )
#include "SoTextureMaskedArea.h"
#include "SoView2DHalo.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDDraw2DSilhouetteInit()
{
  // Register the class in the inventor runtime type system.
  SoTextureMaskedArea::initClass();
  SoView2DHalo::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDDraw2DSilhouetteInit)

