//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDVisibilitySolverInit.cpp
// \author  
// \date    2007-04-16
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDVisibilitySolverSystem.h"
#include <XVBasics.h>
#include "SoVisibilitySolver.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDVisibilitySolverInit()
{
  // Register the class in the inventor runtime type system.
  SoVisibilitySolver::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDVisibilitySolverInit)


