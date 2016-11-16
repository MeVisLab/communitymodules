//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLPDFSystem.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-02-06
*/
//----------------------------------------------------------------------------------


#ifndef _MLPDFSystem_H
#define _MLPDFSystem_H


// DLL export macro definition.
#ifdef MLPDF_EXPORTS
  // Use the MLPDF_EXPORT macro to export classes and functions.
  #define MLPDF_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define MLPDF_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif


#endif // _MLPDFSystem_H
