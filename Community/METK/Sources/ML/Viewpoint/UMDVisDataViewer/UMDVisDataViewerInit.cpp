//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDVisDataViewerInit.cpp
// \author  
// \date    2007-04-26
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDVisDataViewerSystem.h"
#include <XVBasics.h>
#include "SoVisDataViewer.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDVisDataViewerInit()
{
  // Register the class in the inventor runtime type system.
  SoVisDataViewer::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDVisDataViewerInit)


