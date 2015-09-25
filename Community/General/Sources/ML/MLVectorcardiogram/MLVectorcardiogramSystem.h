//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLVectorcardiogramSystem.h
// \author  Axel Newe
// \date    2015-09-25
*/
//----------------------------------------------------------------------------------

#ifndef __MLVectorcardiogramSystem_H
#define __MLVectorcardiogramSystem_H

// DLL export macro definition.
#ifdef MLVECTORCARDIOGRAM_EXPORTS
  // Use the MLVECTORCARDIOGRAM_EXPORT macro to export classes and functions.
  #define MLVECTORCARDIOGRAM_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define MLVECTORCARDIOGRAM_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif


#endif // __MLVectorcardiogramSystem_H
