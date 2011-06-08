//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDViewerTriggerSystem.h
// \author  Konrad Mühler
// \date    2005-04-23
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDVIEWERTRIGGERSYSTEM_H__
#define __UMDVIEWERTRIGGERSYSTEM_H__


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDVIEWERTRIGGER_EXPORTS
    // Export library symbols.
    #define UMDVIEWERTRIGGER_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDVIEWERTRIGGER_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDVIEWERTRIGGER_EXPORTS
  // Use the UMDVIEWERTRIGGER_EXPORT macro to export classes and functions
  #define UMDVIEWERTRIGGER_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDVIEWERTRIGGER_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDVIEWERTRIGGER_EXPORT
  #endif
#endif

#endif
// __UMDVIEWERTRIGGERSYSTEM_H__

