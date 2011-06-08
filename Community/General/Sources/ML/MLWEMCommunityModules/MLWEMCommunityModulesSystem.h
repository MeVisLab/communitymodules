// **InsertLicense** code
//----------------------------------------------------------------------------------
//! Defines export symbols for classes, so they can be used in other DLLs.
/*!
// \file    MLWEMCommunityModulesSystem.h
// \author  Bart De Dobbelaer
// \date    05/2009
*/
//----------------------------------------------------------------------------------

#ifndef __MLWEMCommunityModulesSystem_H
#define __MLWEMCommunityModulesSystem_H

//! Defines export symbols for classes, so they can be used in other DLLs.
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLWEMCOMMUNITYMODULES_EXPORTS
    // Export library symbols.
    #define MLWEMCOMMUNITYMODULES_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLWEMCOMMUNITYMODULES_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLWEMCOMMUNITYMODULES_EXPORTS
  #define MLWEMCOMMUNITYMODULES_EXPORT __declspec(dllexport)
  #else
  #define MLWEMCOMMUNITYMODULES_EXPORT __declspec(dllimport)
  #endif

  #else
  #define MLWEMCOMMUNITYMODULES_EXPORT
  #endif
#endif

#endif // __MLWEMCommunityModulesSystem_H
