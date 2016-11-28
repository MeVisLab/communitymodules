//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    GraphToFibersSystem.h
// \author  Axel Newe
// \date    2016-03-31
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef GRAPHTOFIBERS_EXPORTS
  // Use the GRAPHTOFIBERS_EXPORT macro to export classes and functions.
  #define GRAPHTOFIBERS_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define GRAPHTOFIBERS_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
