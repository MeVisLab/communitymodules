//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    
// \author  Axel Newe
// \date    2015-12-03
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef MLWEMCOLORIZENODES_EXPORTS
  // Use the SMSTEST_EXPORT macro to export classes and functions.
  #define MLWEMCOLORIZENODES_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define MLWEMCOLORIZENODES_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
