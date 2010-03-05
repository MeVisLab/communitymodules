//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCMCurveListSystem.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMCurveListSystem_H
#define __MLCMCurveListSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef MLCMCURVELIST_EXPORTS
// Use the MLCMCURVELIST_EXPORT macro to export classes and functions
#define MLCMCURVELIST_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define MLCMCURVELIST_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define MLCMCURVELIST_EXPORT
#endif


#endif // __MLCMCurveListSystem_H


