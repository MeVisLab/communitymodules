//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCMFieldsSystem.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMFieldsSystem_H
#define __MLCMFieldsSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLCMFIELDS_EXPORTS
    // Export library symbols.
    #define MLCMFIELDS_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLCMFIELDS_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLCMFIELDS_EXPORTS
  // Use the MLCMFIELDS_EXPORT macro to export classes and functions
  #define MLCMFIELDS_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define MLCMFIELDS_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define MLCMFIELDS_EXPORT
  #endif
#endif

#endif // __MLCMFieldsSystem_H


