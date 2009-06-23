//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDInventorPointerInit.cpp
// \author  Konrad
// \date    2006-02-02
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __UMDInventorPointerSystem_H
#include "UMDInventorPointerSystem.h"
#endif

// Include definition of ML_INIT_LIBRARY.
#ifndef __mlLibraryInitMacros_H
#include "mlLibraryInitMacros.h"
#endif

// Include all module headers ...
#ifndef __mlUMDPointerToInventor_H
#include "mlUMDPointerToInventor.h"
#endif

#ifndef __mlUMDInventorToPointer_H
#include "mlUMDInventorToPointer.h"
#endif


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
//----------------------------------------------------------------------------------
int UMDInventorPointerInit (void)
{
  ML_TRACE_IN("UMDInventorPointerInit ()")

  UMDInventorToPointer::initClass();
  UMDPointerToInventor::initClass();  
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(UMDInventorPointerInit)

