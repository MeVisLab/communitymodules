//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDmnOffscreenRendererSystem.h
// \author  Mathias Neugebauer
// \date    2007-03-30
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDMNOFFSCREENRENDERERSYSTEM_H__
#define __UMDMNOFFSCREENRENDERERSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDMNOFFSCREENRENDERER_EXPORTS
// Use the UMDMNOFFSCREENRENDERER_EXPORT macro to export classes and functions
#define UMDMNOFFSCREENRENDERER_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDMNOFFSCREENRENDERER_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDMNOFFSCREENRENDERER_EXPORT
#endif

// Enable usage of PBUFFERS only on windows systems
#ifdef WIN32
  #define USE_WINPBUFFER
#endif


#endif
// __UMDMNOFFSCREENRENDERERSYSTEM_H__

