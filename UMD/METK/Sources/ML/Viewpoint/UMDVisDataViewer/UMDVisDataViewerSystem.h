//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDVisDataViewerSystem.h
// \author  
// \date    2007-04-26
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDVISDATAVIEWERSYSTEM_H__
#define __UMDVISDATAVIEWERSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDVISDATAVIEWER_EXPORTS
// Use the UMDVISDATAVIEWER_EXPORT macro to export classes and functions
#define UMDVISDATAVIEWER_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDVISDATAVIEWER_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDVISDATAVIEWER_EXPORT
#endif


#endif
// __UMDVISDATAVIEWERSYSTEM_H__

