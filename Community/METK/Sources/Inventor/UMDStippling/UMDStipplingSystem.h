//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDStipplingSystem.h
// \author  Alexandra Baer
// \date    2006-07-26
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDSTIPPLINGSYSTEM_H__
#define __UMDSTIPPLINGSYSTEM_H__

#ifdef WIN32

#pragma warning (disable : 4505)

// '__thiscall ...' ist keine Inline-Funktion
#pragma warning (disable : 4710)


#pragma warning( push, 3 )
#endif

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

#ifdef WIN32
#pragma warning( pop )
#endif

// DLL export macro definition
#ifdef WIN32
#ifdef UMDSTIPPLING_EXPORTS
// Use the UMDSTIPPLING_EXPORT macro to export classes and functions
#define UMDSTIPPLING_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDSTIPPLING_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDSTIPPLING_EXPORT
#endif


#endif
// __UMDSTIPPLINGSYSTEM_H__

