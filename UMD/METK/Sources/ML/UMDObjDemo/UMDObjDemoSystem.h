//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDObjDemoSystem.h
// \author  Konrad Mühler
// \date    2005-04-22
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDObjDemoSystem_H
#define __UMDObjDemoSystem_H

#ifdef WIN32
#pragma warning (disable : 4275)
#pragma warning (disable : 4503)
#pragma warning (disable : 4786)
#endif

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDOBJDEMO_EXPORTS
    // Export library symbols.
    #define UMDOBJDEMO_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDOBJDEMO_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDOBJDEMO_EXPORTS
  // Use the UMDOBJDEMO_EXPORT macro to export classes and functions
  #define UMDOBJDEMO_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDOBJDEMO_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDOBJDEMO_EXPORT
  #endif
#endif

#endif // __UMDObjDemoSystem_H


