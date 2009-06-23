//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDSoSceneWriterMDSystem.h
// \author  Konrad Mühler
// \date    2006-05-15
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDSoSceneWriterMDSystem_H
#define __UMDSoSceneWriterMDSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef UMDSOSCENEWRITERMD_EXPORTS
// Use the UMDSOSCENEWRITERMD_EXPORT macro to export classes and functions
#define UMDSOSCENEWRITERMD_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDSOSCENEWRITERMD_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDSOSCENEWRITERMD_EXPORT
#endif


#endif // __UMDSoSceneWriterMDSystem_H


