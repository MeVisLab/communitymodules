//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLOsiriXImporterInit.cpp
// \author  Chunliang Wang
// \date    2009-07-07
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLOsiriXImporterSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "mlOsiriXImporter.h"
#include "mlOsiriXExporter.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLOsiriXImporterInit ()
{
  ML_TRACE_IN("MLOsiriXImporterInit ()")

  OsiriXImporter::initClass();
  // Add initClass calls from all other modules here...
  OsiriXExporter::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLOsiriXImporterInit)

