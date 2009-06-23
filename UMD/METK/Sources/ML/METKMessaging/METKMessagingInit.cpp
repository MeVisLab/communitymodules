//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METKMessagingInit.cpp
// \author  Christian Tietjen
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __METKMessagingSystem_H
#include "METKMessagingSystem.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#include "mlMETKMsgReceiver.h"
#include "mlMETKMsgSender.h"
#include "mlMETKGlobalMessages.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int METKMessagingInit (void)
{
  ML_TRACE_IN("METKMessagingInit ()")

  METKMsgReceiver::initClass();
  METKMsgSender::initClass();
  METKGlobalMessages::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(METKMessagingInit)

