//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDVisDataViewer2DSystem.h
// \author
// \date    2007-04-26
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDVISDATAVIEWER2DSYSTEM_H__
#define __UMDVISDATAVIEWER2DSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDVISDATAVIEWER2D_EXPORTS
// Use the UMDVISDATAVIEWER2D_EXPORT macro to export classes and functions
#define UMDVISDATAVIEWER2D_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDVISDATAVIEWER2D_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDVISDATAVIEWER2D_EXPORT
#endif


#endif
// __UMDVISDATAVIEWER2DSYSTEM_H__

