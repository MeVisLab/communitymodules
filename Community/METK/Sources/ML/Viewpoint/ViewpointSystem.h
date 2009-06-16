//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    ViewpointSystem.h
// \author  Konrad Mühler
// \date    2007-07-26
*/
//----------------------------------------------------------------------------------


#ifndef __ViewpointSystem_H
#define __ViewpointSystem_H

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef VIEWPOINT_EXPORTS
// Use the METKCREATEINTRAOPDUMMY_EXPORT macro to export classes and functions
#define VIEWPOINT_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define VIEWPOINT_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define VIEWPOINT_EXPORT
#endif


#endif // __ViewpointSystem_H


