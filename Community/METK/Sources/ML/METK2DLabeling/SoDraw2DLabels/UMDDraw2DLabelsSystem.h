//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDDraw2DLabelsSystem.h
// \author  Konrad Mühler
// \date    2008-05-08
*/
//----------------------------------------------------------------------------------


#ifndef __UMDDRAW2DLABELSSYSTEM_H__
#define __UMDDRAW2DLABELSSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef UMDDRAW2DLABELS_EXPORTS
// Use the UMDDRAW2DLABELS_EXPORT macro to export classes and functions
#define UMDDRAW2DLABELS_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDDRAW2DLABELS_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDDRAW2DLABELS_EXPORT
#endif


#endif
// __UMDDRAW2DLABELSSYSTEM_H__

