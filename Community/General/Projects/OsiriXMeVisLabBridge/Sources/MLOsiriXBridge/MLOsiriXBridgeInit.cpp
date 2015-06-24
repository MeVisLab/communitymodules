//
//  MLOsiriXBridgeInit.cpp
//  MLOsiriXBridge
//
//  Created by Felix Ritter on 18.01.08.
//  This code is in the public domain.
//

// Local includes
#include "MLOsiriXBridgeSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlOsiriXBridge.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLOsiriXBridgeInit()
{
  ML_TRACE_IN("MLOsiriXBridgeInit()")

  OsiriXBridge::initClass();

  return 1;
}

ML_END_NAMESPACE

//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLOsiriXBridgeInit)
