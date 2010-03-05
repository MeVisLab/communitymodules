//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCMFieldsSystem.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMFieldsSystem_H
#define __MLCMFieldsSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef MLCMFIELDS_EXPORTS
// Use the MLCMFIELDS_EXPORT macro to export classes and functions
#define MLCMFIELDS_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define MLCMFIELDS_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define MLCMFIELDS_EXPORT
#endif


#endif // __MLCMFieldsSystem_H


