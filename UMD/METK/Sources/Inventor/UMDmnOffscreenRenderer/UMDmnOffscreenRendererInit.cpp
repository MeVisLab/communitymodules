//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDmnOffscreenRendererInit.cpp
// \author  Mathias Neugebauer
// \date    2007-03-30
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDmnOffscreenRendererSystem.h"
#include <XVBasics.h>
#include "SomnOffscreenRenderer.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDmnOffscreenRendererInit()
{
  // Register the class in the inventor runtime type system.
  SomnOffscreenRenderer::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDmnOffscreenRendererInit)


