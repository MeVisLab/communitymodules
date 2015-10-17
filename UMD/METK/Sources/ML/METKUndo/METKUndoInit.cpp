//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METKUndoInit.cpp
// \author  kmuehler
// \date    2008-06-19
*/
//----------------------------------------------------------------------------------


// Local includes
#include "METKUndoSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlMETKUndo.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int METKUndoInit ()
{
  ML_TRACE_IN("METKUndoInit ()")

  METKUndo::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(METKUndoInit)

