//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDBoundingBox2System.h
// \author  Konrad Mühler
// \date    2005-05-10
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDBoundingBox2System_H
#define __UMDBoundingBox2System_H


#ifdef WIN32
#pragma warning (disable : 4275)
#pragma warning (disable : 4503)
#pragma warning (disable : 4786)
#endif

// DLL export macro definition
#ifdef WIN32
#ifdef UMDBOUNDINGBOX2_EXPORTS
// Use the UMDBOUNDINGBOX2_EXPORT macro to export classes and functions
#define UMDBOUNDINGBOX2_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDBOUNDINGBOX2_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDBOUNDINGBOX2_EXPORT
#endif


#endif // __UMDBoundingBox2System_H


