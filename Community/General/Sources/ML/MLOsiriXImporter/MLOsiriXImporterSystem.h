//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLOsiriXImporterSystem.h
// \author  Chunliang Wang
// \date    2009-07-07
*/
//----------------------------------------------------------------------------------


#ifndef __MLOsiriXImporterSystem_H
#define __MLOsiriXImporterSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef MLOSIRIXIMPORTER_EXPORTS
// Use the MLOSIRIXIMPORTER_EXPORT macro to export classes and functions
#define MLOSIRIXIMPORTER_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define MLOSIRIXIMPORTER_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define MLOSIRIXIMPORTER_EXPORT
#endif


#endif // __MLOsiriXImporterSystem_H


