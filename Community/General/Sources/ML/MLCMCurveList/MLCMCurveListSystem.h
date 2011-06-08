//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCMCurveListSystem.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMCurveListSystem_H
#define __MLCMCurveListSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLCMCURVELIST_EXPORTS
    // Export library symbols.
    #define MLCMCURVELIST_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLCMCURVELIST_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLCMCURVELIST_EXPORTS
  // Use the MLCMCURVELIST_EXPORT macro to export classes and functions
  #define MLCMCURVELIST_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define MLCMCURVELIST_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define MLCMCURVELIST_EXPORT
  #endif
#endif

#endif // __MLCMCurveListSystem_H


