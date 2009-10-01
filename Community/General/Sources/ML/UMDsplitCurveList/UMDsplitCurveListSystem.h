//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDsplitCurveListSystem.h
// \author  Tobias Mönch
// \date    2009-08-05
*/
//----------------------------------------------------------------------------------


#ifndef __UMDsplitCurveListSystem_H
#define __UMDsplitCurveListSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef UMDSPLITCURVELIST_EXPORTS
// Use the UMDSPLITCURVELIST_EXPORT macro to export classes and functions
#define UMDSPLITCURVELIST_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDSPLITCURVELIST_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDSPLITCURVELIST_EXPORT
#endif


#endif // __UMDsplitCurveListSystem_H


