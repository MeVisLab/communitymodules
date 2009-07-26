//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    CVLAscendingAortaDetectionSystem.h
// \author  Stefan Saur
// \date    2007-03-24
//
*/
//----------------------------------------------------------------------------------


#ifndef __CVLAscendingAortaDetectionSystem_H
#define __CVLAscendingAortaDetectionSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef CVLASCENDINGAORTADETECTION_EXPORTS
// Use the CVLASCENDINGAORTADETECTION_EXPORT macro to export classes and functions
#define CVLASCENDINGAORTADETECTION_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define CVLASCENDINGAORTADETECTION_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define CVLASCENDINGAORTADETECTION_EXPORT
#endif


#endif // __CVLAscendingAortaDetectionSystem_H


