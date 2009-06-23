//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDVisibilitySolverSystem.h
// \author  
// \date    2007-04-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDVISIBILITYSOLVERSYSTEM_H__
#define __UMDVISIBILITYSOLVERSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDVISIBILITYSOLVER_EXPORTS
// Use the UMDVISIBILITYSOLVER_EXPORT macro to export classes and functions
#define UMDVISIBILITYSOLVER_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDVISIBILITYSOLVER_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDVISIBILITYSOLVER_EXPORT
#endif


#endif
// __UMDVISIBILITYSOLVERSYSTEM_H__

