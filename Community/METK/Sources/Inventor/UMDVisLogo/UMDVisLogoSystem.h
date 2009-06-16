//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDVisLogoSystem.h
// \author  Konrad Mühler
// \date    2007-06-12
*/
//----------------------------------------------------------------------------------


#ifndef __UMDVISLOGOSYSTEM_H__
#define __UMDVISLOGOSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDVISLOGO_EXPORTS
// Use the UMDVISLOGO_EXPORT macro to export classes and functions
#define UMDVISLOGO_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDVISLOGO_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDVISLOGO_EXPORT
#endif


#endif
// __UMDVISLOGOSYSTEM_H__

