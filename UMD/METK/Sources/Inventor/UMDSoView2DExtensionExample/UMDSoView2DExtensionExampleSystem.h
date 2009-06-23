//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDSoView2DExtensionExampleSystem.h
// \author  Konrad Mühler
// \date    2008-01-07
*/
//----------------------------------------------------------------------------------


#ifndef __UMDSOVIEW2DEXTENSIONEXAMPLESYSTEM_H__
#define __UMDSOVIEW2DEXTENSIONEXAMPLESYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORTS
// Use the UMDVIEW2DEXTENSIONEXAMPLE_EXPORT macro to export classes and functions
#define UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORT
#endif


#endif
// __UMDSOVIEW2DEXTENSIONEXAMPLESYSTEM_H__

