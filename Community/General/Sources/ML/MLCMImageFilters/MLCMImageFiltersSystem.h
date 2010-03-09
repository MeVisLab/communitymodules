//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCMImageFiltersSystem.h
// \author  Theo van Walsum
// \date    2007-08-14
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMImageFiltersSystem_H
#define __MLCMImageFiltersSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef MLCMIMAGEFILTERS_EXPORTS
// Use the MLCMIMAGEFILTERS_EXPORT macro to export classes and functions
#define MLCMIMAGEFILTERS_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define MLCMIMAGEFILTERS_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define MLCMIMAGEFILTERS_EXPORT
#endif


#endif // __MLCMImageFiltersSystem_H


