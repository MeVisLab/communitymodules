//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    AnimationSystem.h
// \author  Konrad Mühler
// \date    2007-06-28
//
*/
//----------------------------------------------------------------------------------


#ifndef __AnimationSystem_H
#define __AnimationSystem_H

#ifdef WIN32
#pragma warning (disable : 4702)
#endif

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#ifdef WIN32
#ifdef ANIMATION_EXPORTS
// Use the UMDANIMATION2_EXPORT macro to export classes and functions
#define ANIMATION_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define ANIMATION_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define ANIMATION_EXPORT
#endif

#endif // __AnimationSystem_H


