// **InsertLicense** code
//----------------------------------------------------------------------------------
//! Defines export symbols for classes, so they can be used in other DLLs.
/*!
// \file    MLCSOCommunityModulesSystem.h
// \author  Bart De Dobbelaer
// \date    05/2009
*/
//----------------------------------------------------------------------------------

#ifndef __MLCSOCommunityModulesSystem_H
#define __MLCSOCommunityModulesSystem_H

//! Defines export symbols for classes, so they can be used in other DLLs.
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLCSOCOMMUNITYMODULES_EXPORTS
    // Export library symbols.
    #define MLCSOCOMMUNITYMODULES_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLCSOCOMMUNITYMODULES_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLCSOCOMMUNITYMODULES_EXPORTS
  #define MLCSOCOMMUNITYMODULES_EXPORT __declspec(dllexport)
  #else
  #define MLCSOCOMMUNITYMODULES_EXPORT __declspec(dllimport)
  #endif

  #else
  #define MLCSOCOMMUNITYMODULES_EXPORT
  #endif
#endif

#endif // __MLCSOCommunityModulesSystem_H
