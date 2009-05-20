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


#ifdef WIN32
#ifdef MLCSOCOMMUNITYMODULES_EXPORTS
#define MLCSOCOMMUNITYMODULES_EXPORT __declspec(dllexport)
#else
#define MLCSOCOMMUNITYMODULES_EXPORT __declspec(dllimport)
#endif

#else
#define MLCSOCOMMUNITYMODULES_EXPORT
#endif


#endif // __MLCSOCommunityModulesSystem_H
