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


#endif // __MLXMarkerListCommunityModulesSystem_H


