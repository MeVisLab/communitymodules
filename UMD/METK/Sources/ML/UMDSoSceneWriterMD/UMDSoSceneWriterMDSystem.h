//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDSoSceneWriterMDSystem.h
// \author  Konrad Mühler
// \date    2006-05-15
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDSoSceneWriterMDSystem_H
#define __UMDSoSceneWriterMDSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDSOSCENEWRITERMD_EXPORTS
    // Export library symbols.
    #define UMDSOSCENEWRITERMD_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDSOSCENEWRITERMD_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDSOSCENEWRITERMD_EXPORTS
  // Use the UMDSOSCENEWRITERMD_EXPORT macro to export classes and functions
  #define UMDSOSCENEWRITERMD_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDSOSCENEWRITERMD_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDSOSCENEWRITERMD_EXPORT
  #endif
#endif

#endif // __UMDSoSceneWriterMDSystem_H


