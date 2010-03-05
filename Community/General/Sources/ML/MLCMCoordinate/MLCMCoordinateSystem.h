//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCMCoordinateSystem.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMCoordinateSystem_H
#define __MLCMCoordinateSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef MLCMCOORDINATE_EXPORTS
// Use the MLCMCoordinate_EXPORT macro to export classes and functions
#define MLCMCOORDINATE_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define MLCMCOORDINATE_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define MLCMCOORDINATE_EXPORT
#endif


#endif // __MLCMCoordinateSystem_H


