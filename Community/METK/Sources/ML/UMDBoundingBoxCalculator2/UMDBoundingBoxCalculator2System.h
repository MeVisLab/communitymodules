//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDBoundingBoxCalculator2System.h
// \author  Konrad
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDBoundingBoxCalculator2System_H
#define __UMDBoundingBoxCalculator2System_H


// DLL export macro definition
#ifdef WIN32
#ifdef UMDBOUNDINGBOXCALCULATOR2_EXPORTS
// Use the UMDBOUNDINGBOXCALCULATOR2_EXPORT macro to export classes and functions
#define UMDBOUNDINGBOXCALCULATOR2_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDBOUNDINGBOXCALCULATOR2_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDBOUNDINGBOXCALCULATOR2_EXPORT
#endif


#endif // __UMDBoundingBoxCalculator2System_H


