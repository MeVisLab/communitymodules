//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCSOToRTStructureSystem.h
// \author  linal
// \date    2010-01-20
*/
//----------------------------------------------------------------------------------


#ifndef __MLCSOToRTStructureSystem_H
#define __MLCSOToRTStructureSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef MLCSOTORTSTRUCTURE_EXPORTS
// Use the MLCSOTORTSTRUCTURE_EXPORT macro to export classes and functions
#define MLCSOTORTSTRUCTURE_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define MLCSOTORTSTRUCTURE_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define MLCSOTORTSTRUCTURE_EXPORT
#endif


#endif // __MLCSOToRTStructureSystem_H


