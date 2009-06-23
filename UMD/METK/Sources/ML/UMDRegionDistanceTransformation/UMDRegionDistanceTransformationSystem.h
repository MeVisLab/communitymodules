//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDRegionDistanceTransformationSystem.h
// \author  Konrad Muehler
// \date    2009-03-17
*/
//----------------------------------------------------------------------------------


#ifndef __UMDRegionDistanceTransformationSystem_H
#define __UMDRegionDistanceTransformationSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef UMDREGIONDISTANCETRANSFORMATION_EXPORTS
// Use the UMDREGIONDISTANCETRANSFORMATION_EXPORT macro to export classes and functions
#define UMDREGIONDISTANCETRANSFORMATION_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDREGIONDISTANCETRANSFORMATION_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDREGIONDISTANCETRANSFORMATION_EXPORT
#endif


#endif // __UMDRegionDistanceTransformationSystem_H


