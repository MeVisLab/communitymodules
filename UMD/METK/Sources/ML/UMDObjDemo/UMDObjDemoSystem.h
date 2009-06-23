//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDObjDemoSystem.h
// \author  Konrad Mühler
// \date    2005-04-22
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDObjDemoSystem_H
#define __UMDObjDemoSystem_H

#ifdef WIN32
#pragma warning (disable : 4275)
#pragma warning (disable : 4503)
#pragma warning (disable : 4786)
#endif

// DLL export macro definition
#ifdef WIN32
#ifdef UMDOBJDEMO_EXPORTS
// Use the UMDOBJDEMO_EXPORT macro to export classes and functions
#define UMDOBJDEMO_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDOBJDEMO_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDOBJDEMO_EXPORT
#endif


#endif // __UMDObjDemoSystem_H


