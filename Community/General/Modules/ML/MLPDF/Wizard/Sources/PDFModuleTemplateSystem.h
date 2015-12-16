//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    ${__TARGETFILE__}
// \author  ${vars.author}
// \date    ${__DATE__}
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef ${vars.cppProject.fullProjectUpper}_EXPORTS
  // Use the ${vars.cppProject.fullProjectUpper}_EXPORT macro to export classes and functions.
  #define ${vars.cppProject.fullProjectUpper}_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define ${vars.cppProject.fullProjectUpper}_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
