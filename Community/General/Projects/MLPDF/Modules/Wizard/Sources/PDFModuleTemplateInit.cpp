//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    ${__TARGETFILE__}
// \author  ${vars.author}
// \date    ${__DATE__}
*/
//----------------------------------------------------------------------------------


// Local includes
#include "${vars.cppProject.fullProject}System.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "${vars.moduleName}.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int ${vars.cppProject.fullProject}Init()
{
  // Add initClass calls from modules here.
  ${vars.moduleName}::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(${vars.cppProject.fullProject}Init)