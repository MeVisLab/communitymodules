//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDViewerTriggerInit.cpp
// \author  Konrad Mühler
// \date    2005-07-06
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDViewerTriggerSystem.h"
#include <XVBasics.h>
#include "SoUMDViewerTrigger.h"


//! Called on initialization time of this dll. Register all new stuff.
void UMDViewerTriggerInit()
{
  //! Register the class in the inventor runtime type system.
  SoUMDViewerTrigger::initClass();
};

//! Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDViewerTriggerInit)


