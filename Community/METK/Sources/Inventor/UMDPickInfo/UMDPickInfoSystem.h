//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDPickInfoSystem.h
// \author  Stefan Hiller
// \date    2007-09-11
*/
//----------------------------------------------------------------------------------


#ifndef __UMDPICKINFOSYSTEM_H__
#define __UMDPICKINFOSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDPICKINFO_EXPORTS
// Use the UMDPICKINFO_EXPORT macro to export classes and functions
#define UMDPICKINFO_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDPICKINFO_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDPICKINFO_EXPORT
#endif


#endif
// __UMDPICKINFOSYSTEM_H__

