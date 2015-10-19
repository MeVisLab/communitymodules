//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDStraightLineInit.cpp
// \author  kmuehler
// \date    2008-08-06
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDStraightLineSystem.h"
#include <XVBasics.h>
#include "SoStraightLine.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDStraightLineInit()
{
  // Register the class in the inventor runtime type system.
  SoStraightLine::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDStraightLineInit)


