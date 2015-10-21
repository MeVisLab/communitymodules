//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLBaseListExtensionsSystem.h 
// \author  Axel Newe
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------

#ifndef __MLBaseListExtensionsSystem_H
#define __MLBaseListExtensionsSystem_H

// DLL export macro definition.
#ifdef MLBASELISTEXTENSIONS_EXPORTS
  // Use the MLBASELISTEXTENSIONS_EXPORT macro to export classes and functions.
  #define MLBASELISTEXTENSIONS_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define MLBASELISTEXTENSIONS_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif

#endif // __MLBaseListExtensionsSystem_H
