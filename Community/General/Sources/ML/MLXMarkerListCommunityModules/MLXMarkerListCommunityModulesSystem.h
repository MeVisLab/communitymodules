//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLXMarkerListCommunityModulesSystem.h
// \author  Coert Metz
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLXMarkerListCommunityModulesSystem_H
#define __MLXMarkerListCommunityModulesSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLXMARKERLISTCOMMUNITYMODULES_EXPORTS
    // Export library symbols.
    #define MLXMARKERLISTCOMMUNITYMODULES_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLXMARKERLISTCOMMUNITYMODULES_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLXMARKERLISTCOMMUNITYMODULES_EXPORTS
  // Use the MLXMARKERLISTCOMMUNITYMODULES_EXPORT macro to export classes and functions
  #define MLXMARKERLISTCOMMUNITYMODULES_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define MLXMARKERLISTCOMMUNITYMODULES_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define MLXMARKERLISTCOMMUNITYMODULES_EXPORT
  #endif
#endif

#endif // __MLXMarkerListCommunityModulesSystem_H


