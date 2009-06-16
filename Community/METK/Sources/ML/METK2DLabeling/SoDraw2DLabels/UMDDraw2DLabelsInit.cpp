//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDDraw2DLabelsInit.cpp
// \author  Konrad Mühler
// \date    2008-05-08
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDDraw2DLabelsSystem.h"
#include <XVBasics.h>
#include "SoDraw2DLabels.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDDraw2DLabelsInit()
{
  // Register the class in the inventor runtime type system.
  SoDraw2DLabels::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDDraw2DLabelsInit)


