//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLTestModulesSystem.h
// \author  Wolf Spindler
// \date    2009-05-19
*/
//----------------------------------------------------------------------------------


#ifndef __MLTestModulesSystem_H
#define __MLTestModulesSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef MLTESTMODULES_EXPORTS
// Use the MLTESTMODULES_EXPORT macro to export classes and functions
#define MLTESTMODULES_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define MLTESTMODULES_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define MLTESTMODULES_EXPORT
#endif


#endif // __MLTestModulesSystem_H


