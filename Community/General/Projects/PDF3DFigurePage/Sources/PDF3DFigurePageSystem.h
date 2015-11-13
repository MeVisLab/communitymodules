//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    PDF3DFigurePageSystem.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef PDF3DFIGUREPAGE_EXPORTS
  // Use the SMSPDF3DFIGURE_EXPORT macro to export classes and functions.
  #define PDF3DFIGUREPAGE_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define PDF3DFIGUREPAGE_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
