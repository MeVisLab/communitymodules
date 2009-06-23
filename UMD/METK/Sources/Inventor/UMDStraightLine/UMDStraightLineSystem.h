//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDStraightLineSystem.h
// \author  kmuehler
// \date    2008-08-06
*/
//----------------------------------------------------------------------------------


#ifndef __UMDSTRAIGHTLINESYSTEM_H__
#define __UMDSTRAIGHTLINESYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDSTRAIGHTLINE_EXPORTS
// Use the UMDSTRAIGHTLINE_EXPORT macro to export classes and functions
#define UMDSTRAIGHTLINE_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDSTRAIGHTLINE_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDSTRAIGHTLINE_EXPORT
#endif


#endif
// __UMDSTRAIGHTLINESYSTEM_H__

