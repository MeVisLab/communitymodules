//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDObjDemoInit.cpp
// \author  Konrad Mühler
// \date    2005-04-22
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __UMDObjDemoSystem_H
#include "UMDObjDemoSystem.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#ifndef __mlUMDObjDemo_H
#include "mlUMDObjDemo.h"
#endif


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int UMDObjDemoInit (void)
{
  UMDObjDemo::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(UMDObjDemoInit)