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


#ifdef WIN32
#ifdef MLWEMCOMMUNITYMODULES_EXPORTS
#define MLWEMCOMMUNITYMODULES_EXPORT __declspec(dllexport)
#else
#define MLWEMCOMMUNITYMODULES_EXPORT __declspec(dllimport)
#endif

#else
#define MLWEMCOMMUNITYMODULES_EXPORT
#endif


#endif // __MLWEMCommunityModulesSystem_H
