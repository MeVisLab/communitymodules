//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    PDF3DWorkSystem.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-20
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef PDF3DWORK_EXPORTS
  // Use the SMSTEST_EXPORT macro to export classes and functions.
#define PDF3DWORK_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
#define PDF3DWORK_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
