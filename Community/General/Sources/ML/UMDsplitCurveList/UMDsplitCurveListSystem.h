//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDsplitCurveListSystem.h
// \author  Tobias Mönch
// \date    2009-08-05
*/
//----------------------------------------------------------------------------------


#ifndef __UMDsplitCurveListSystem_H
#define __UMDsplitCurveListSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDSPLITCURVELIST_EXPORTS
    // Export library symbols.
    #define UMDSPLITCURVELIST_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDSPLITCURVELIST_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDSPLITCURVELIST_EXPORTS
  // Use the UMDSPLITCURVELIST_EXPORT macro to export classes and functions
  #define UMDSPLITCURVELIST_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDSPLITCURVELIST_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDSPLITCURVELIST_EXPORT
  #endif
#endif

#endif // __UMDsplitCurveListSystem_H


